/*
	Filename: OggStream.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates an OGG stream playback
	Comments: As it is not natively supported by OPENAL, if you want to play OGG files, you have to
	use OGG-Vorbis library
	Attribution: OGG playback is based in tutorial on http://www.devmaster.net/articles/openal-tutorials/lesson8.php
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "OggStream.h"
#include "SoundManager.h"

//Playback function - Play with loop 
bool OggStream::Playback(bool loop)
{
	//Setup loop variable
	mLoop = loop;

	//IF - it is playing dont do nothing
    if(IsPlaying())
        return true;
        
	//Check if buffers were processed
	int numprocessed(0);
	alGetSourcei(mSource,AL_BUFFERS_PROCESSED,&numprocessed);
	_check("Getting Source parameters");
	//IF - No buffers processed, that is, empty of data (first load-up)
	if(numprocessed == 0)
	{
		//Setup streaming buffers
		if(!_stream(mBuffers[0]))
			return false;
 
		if(!_stream(mBuffers[1]))
			return false;

		//Queue buffers and play source
		alSourceQueueBuffers(mSource, 2, mBuffers);
		_check("Queuing buffers");
	}

    alSourcePlay(mSource);
	mPlay = true;
	mStop = false;
   
    return true;
}

//Playback function - Stop playback
void OggStream::Stop(bool gotostart)
{
	mStop = true;
	mRestartTrack = gotostart;
}

//Returns if audio is playing
bool OggStream::IsPlaying()
{
    ALenum state;
    
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    _check("Getting Source Parameters");
    return (state == AL_PLAYING);
}

//Returns if audio is stopped
bool OggStream::IsStopped()
{
    ALenum state;
    
    alGetSourcei(mSource, AL_SOURCE_STATE, &state);
    _check("Getting Source Parameters");
    return (state != AL_PLAYING 
		    &&
			state != AL_PAUSED);
}

//Update streaming and execution of audio
void OggStream::Update()
{
    int processed;
    bool active = true;
 
	//IF - Check if prompted to stop playing
	if(!mStop)
	{
		//Check if it should be playing
		if(mPlay)
		{
			//Check good processing of OpenAL buffers allways
			if(!IsPlaying())
			{
				//Source can stop playing because:
				//Buffers ended
				//OR
				//App changed conFtext, so source stops automatically
				
				//IF - There are buffers pending in data, resume playing
				if(IsStopped())
				{
					//Just play it again
					alSourcePlay(mSource);
				}
				else
				{
					Playback(true);  //Resend buffers if finished or recommand to play
				}
			}

			alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &processed);

			while(processed > 0)
			{
				ALuint buffer;
		        
				alSourceUnqueueBuffers(mSource, 1, &buffer);
				 _check("Unqueuing buffers");
		 
				active = _stream(buffer);
		 
				alSourceQueueBuffers(mSource, 1, &buffer);
				_check("Queuing buffers");

				processed--;
			}
		
			//IF - Finished but we have to loop
		   if(!active && mLoop) 
		   {
			   //Go back to start of file...
			   ov_time_seek(&mOggStream,0);
			   Playback(true);
		   }
	   }

	}
	else //ELSE - Prompted to stop playing
	{
		if(IsPlaying())
		{
			//Clear OpenAL state
			//Stop OpenAL source
			alSourceStop(mSource);
			//Empty buffers
			_empty();
		}

		if(mRestartTrack)
		{
			//Go back to start of file...
			ov_time_seek(&mOggStream,0);
			mTimePosition = 0.0;
			mRestartTrack = false;
		}
	}
}

void OggStream::_display()
{
	//std::stringstream
    /*cout
        << "version         " << mVorbisInfo->version         << "\n"
        << "channels        " << mVorbisInfo->channels        << "\n"
        << "rate (hz)       " << mVorbisInfo->rate            << "\n"
        << "bitrate upper   " << mVorbisInfo->bitrate_upper   << "\n"
        << "bitrate nominal " << mVorbisInfo->bitrate_nominal << "\n"
        << "bitrate lower   " << mVorbisInfo->bitrate_lower   << "\n"
        << "bitrate window  " << mVorbisInfo->bitrate_window  << "\n"
        << "\n"
        << "vendor " << mVorbisComment->vendor << "\n";
        
    for(int i = 0; i < mVorbisComment->comments; i++)
        cout << "   " << mVorbisComment->user_comments[i] << "\n";
        
    cout << endl;*/
}

//Open file
void OggStream::_open(const std::string &path)
{
	SingletonLogMgr::Instance()->AddNewLine("OggStream::open","Opening ogg file" + path,LOGNORMAL); 
    int result; 
    mOggFile = fopen(path.c_str(), "rb");
    if(!mOggFile)
		throw GenericException("Could not open Ogg file.",GenericException::FILE_NOT_FOUND);
 
	//CRASHES!!!! DOCS SAY TO USE ov_open_callbacks
    //if((result = ov_open(mOggFile, &mOggStream, NULL, 0)) < 0)
	if((result = ov_open_callbacks(mOggFile,&mOggStream,NULL,0,OV_CALLBACKS_DEFAULT)) < 0)
    {
        fclose(mOggFile);
		throw GenericException("Could not open Ogg stream. " + _errorString(result),GenericException::LIBRARY_ERROR);
    }
    
	mVorbisInfo = ov_info(&mOggStream, -1);
    mVorbisComment = ov_comment(&mOggStream, -1);
 
    if(mVorbisInfo->channels == 1)
        mFormat = AL_FORMAT_MONO16;
    else
        mFormat = AL_FORMAT_STEREO16;

	alGenBuffers(2, mBuffers);
    _check("Generating buffers");

    alSource3f(mSource, AL_POSITION,        0.0, 0.0, 0.0);
    alSource3f(mSource, AL_VELOCITY,        0.0, 0.0, 0.0);
    alSource3f(mSource, AL_DIRECTION,       0.0, 0.0, 0.0);
    alSourcef (mSource, AL_ROLLOFF_FACTOR,  0.0          );
    alSourcei (mSource, AL_SOURCE_RELATIVE, AL_TRUE      );
	
	SingletonLogMgr::Instance()->AddNewLine("OggStream::open","File" + path + "Read and ready to play",LOGNORMAL); 
}

//Stream audio contents to buffer and OpenAl
bool OggStream::_stream(ALuint buffer)
{
	//Local variables
    char data[STREAMBUFFERSIZE];
    int  size = 0;
    int  section;
    int  result;
 
	//Fill buffer
    while(size < STREAMBUFFERSIZE)
    {
        result = ov_read(&mOggStream, data + size, STREAMBUFFERSIZE - size, 0, 2, 1, & section);
    
        if(result > 0)
            size += result;
        else
            if(result < 0)
                throw GenericException(_errorString(result),GenericException::LIBRARY_ERROR);
            else
                break;
    }
    
	//There is nothing else to read
    if(size == 0)
        return false;

	//Update location of time in file
	mTimePosition =  ov_time_tell(&mOggStream);

	//Copy contents to buffer data on OpenAL
    alBufferData(buffer, mFormat, data, size, mVorbisInfo->rate);
    _check("Copying buffer data");
 
    return true;
}

//Release all resources
void OggStream::_release()
{
	//Stop OpenAL source
    alSourceStop(mSource);
	//Empty buffers
    _empty();
    alDeleteBuffers(1, mBuffers);
    _check("Deleting Buffers");

	SingletonLogMgr::Instance()->AddNewLine("OggStream::_release","Ogg Stream resources released and file closed",LOGDEBUG); 
}

//Release OpenAl data
void OggStream::_empty()
{
    int queued;
    
    alGetSourcei(mSource, AL_BUFFERS_QUEUED, &queued);
    int control(0);
    while(queued--)
    {
        ALuint buffer;
    
        alSourceUnqueueBuffers(mSource, 1, &buffer);
        _check("Unqueing buffers");

		control++;
		if(control > 2)
			break;
    }
}

//Check openAl error
bool OggStream::_check(const std::string& logmsgonerror)
{
    int error (alGetError());
    if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("OggStream::_check","OpenAl error(" + logmsgonerror +"): " + SingletonSoundMgr::Instance()->GetALErrorString(error,false),LOGEXCEPTION);
		return false;
	} 

	return true;
}

//decode error code to string description
std::string OggStream::_errorString(int code)
{
    switch(code)
    {
        case OV_EREAD:
            return std::string("Read from media.");
        case OV_ENOTVORBIS:
			return std::string("Not Vorbis data.");
        case OV_EVERSION:
            return std::string("Vorbis version mismatch.");
        case OV_EBADHEADER:
            return std::string("Invalid Vorbis header.");
        case OV_EFAULT:
            return std::string("Internal logic fault (bug or heap/stack corruption.");
        default:
            return std::string("Unknown Ogg error.");
    }
}