/*
	Filename: OggStream.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates an OGG stream playback
	Comments: As it is not natively supported by OPENAL, if you want to play OGG files, you have to
	use OGG-Vorbis library
	It is also derived from and "Audio" object, so it implements play/stop differently
	Attribution: OGG playback is based in tutorial on http://www.devmaster.net/articles/openal-tutorials/lesson8.php
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _OGGSTREAM
#define _OGGSTREAM

//Library dependencies
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <string>
#include <sstream>
#include "al.h"   //OPENAL

//Classes dependencies
#include "GenericException.h"
#include "LogManager.h"

class SoundManager;

//Definitions

class OggStream
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	OggStream(const std::string &filepath, int sourceid)
		:mOggFile(NULL),
		mVorbisInfo(NULL),
		mVorbisComment(NULL),
		mFileName(filepath),
		mLoop(false),
		mPlay(false),
		mStop(false),
		mRestartTrack(false),
		mSource(sourceid),
		mTimePosition(0)
	{
		//Open file and prepare audio to play
		_open(filepath);	
		SingletonLogMgr::Instance()->AddNewLine("OggStream","Ogg Stream Object created",LOGDEBUG);
	}
	~OggStream()
	{
		//Release resources
		_release();
		//Close file and delete temporal data
		ov_clear(&mOggStream);
		SingletonLogMgr::Instance()->AddNewLine("OggStream","Ogg Stream Object destructed",LOGDEBUG);
	}
	//----- GET/SET FUNCTIONS -----
	const std::string &GetFilename() { return mFileName; }
	double GetProcessedTime() { return mTimePosition; }
	ALuint GetSource() { return mSource; }
	//----- OTHER FUNCTIONS -----
	void Stop(bool gotostart);					// Stop playing the Ogg stream
	bool Playback(bool loop);				// play the Ogg stream	
	bool IsPlaying();				// check if the source is playing
	bool IsStopped();				// Check if the source is stopped
    void Update();					// update the stream if necessary
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	FILE*           mOggFile;       // file handle
	std::string		mFileName;		//File name
    OggVorbis_File  mOggStream;     // stream handle
    vorbis_info*    mVorbisInfo;    // some formatting data
    vorbis_comment* mVorbisComment; // user comments
	double			mTimePosition;	// Time where file is playing

	bool mLoop;			//Loop stream once finished (replay)
	bool mPlay;			//Tracks if stream is playing
	bool mStop;			//Memorizes request to stop playing
	bool mRestartTrack;	//Memorizes request to restart track from beginning

    ALuint mBuffers[2]; // front and back buffers for audio streaming
    ALuint mSource;     // audio source
    ALenum mFormat;     // internal format
	
	//----- INTERNAL FUNCTIONS -----
	void _open(const std::string &path);	//Open file   
	void _display();					// display some info on the file
    bool _stream(ALuint buffer);   // reloads a buffer
    void _empty();                 // empties the queue
	bool _check(const std::string& logmsgonerror);                 // checks OpenAL error state
	void _release();				  // release the file handle
	std::string _errorString(int code); // stringify an error code
};

#endif