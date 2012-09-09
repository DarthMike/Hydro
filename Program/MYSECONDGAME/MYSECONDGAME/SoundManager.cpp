/*
	Filename: SoundManager.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates managing of sound system
	Comments: 
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "alc.h"
#include "alut.h"
#include "SoundManager.h"

//Loads a sound directly from file
bool SoundManager::LoadSound( const std::string& file, const std::string& actionname )
{
	//Search for registerd action
	BuffersMap::iterator it = mBuffers.find( actionname );
	if( it == mBuffers.end() )
	{
		IdBuffer idBuffer;
		if( _loadSound(file, idBuffer) )
		{
			mBuffers.insert(std::pair<Action, IdBuffer>(actionname, idBuffer) );
			return true;
		}
	}

	return false;
}

//Unloads a sound
bool SoundManager::DeleteSound( const std::string& actionname )
{
	//Search for registerd action
	BuffersMap::iterator it = mBuffers.find( actionname );
	if( it != mBuffers.end() )
	{
		//Check if there is any source related with this action and unbind it
		ALuint srcid;   //The source id
		int index(0);   //The index in vector
		while((index = _getSourcePlayingAction(actionname,srcid)) != -1)
		{
			//Unbind source playing action
			_unbindSourceToBuffer(index);

			//Check if there was a fadeout and clear it
			//LOOP - Search for source fading out
			FadeOutSources::iterator fadeit;
			//LOOP - Decrement sources gain
			for(fadeit=mFadeOutSources.begin();fadeit!=mFadeOutSources.end();++fadeit)
			{
				if((*fadeit).source == srcid)
				{
					mFadeOutSources.erase(fadeit);
					break;
				}
			}//LOOP END				
		}

		//Delete the buffer
		if(!_deleteSound(it->second))
		{
			return false;
		}
		else
		{
			//Delete the contained Id
			mBuffers.erase(it);
		}
	}
	else
	{
		//Error
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::DeleteSound","Intent to delete non-existent sound action:" + actionname,LOGEXCEPTION);
	}

	//Ok
	return true;
}

//Loads a sound stream
bool SoundManager::LoadSoundStream( const std::string& file, const std::string& name )
{
	//Search for registerd stream action
	StreamBuffersMap::iterator it = mStreamBuffers.find( name );
	if( it == mStreamBuffers.end() )
	{
		//Check if there are sources free to play music in
		int index (_getSource(true));  //Reserve this sound for this music stream
	
		if (index==-1)
			return false;

		//Create new stream and store it
		OggStream* newstream = new OggStream(file,mSources[index].source);
		mStreamBuffers[name] = newstream;
	}

	return true;
}

//Unloads a sound stream
bool SoundManager::DeleteSoundStream( const std::string& name )
{
	//Search for registerd stream action
	StreamBuffersMap::iterator it = mStreamBuffers.find( name );
	if( it != mStreamBuffers.end() )
	{
		//Memorize source id
		ALuint source (it->second->GetSource());
		//Stop sound stream
		StopMusic(name,true);

		//Delete dynamic memory
		delete (it->second);
		mStreamBuffers.erase(it);

		//Uncheck reservation for this source
		SourcesVector::iterator srcitr;
		//LOOP - Check all sources
		for(srcitr = mSources.begin();srcitr != mSources.end();++srcitr)
		{
			if((*srcitr).source == source)
			{
				(*srcitr).actionplayed = "";
				(*srcitr).reserved = false;
			}
		}//LOOP END
	}
	else
	{
		//Error
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::DeleteSoundStream","Intent to delete non-existent sound stream: '" + name + "'",LOGEXCEPTION);
		return false;
	}

	//Ok
	return true;
}

//Sets global gain for sound
void SoundManager::SetGain(float gain)
{
	alListenerf(AL_GAIN,gain);
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetGain","Failure while changing main volume (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
}

//Gets global gain for sound
float SoundManager::GetGain()
{
	float gain;
	alGetListenerf(AL_GAIN,&gain);
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetGain","Failure while changing main volume (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	return gain;
}

//Increments general volume
void SoundManager::VolumeUp(float amount)
{
	//Increment volume in "LISTENER"
	
	//Local variables
	float CurrentGain;

	//Value to increment
	CurrentGain = GetGain();
    //Increment
	CurrentGain += amount;
	//Limits
	if (CurrentGain > GAINLIMIT)
	{
		CurrentGain = GAINLIMIT;
	}
	//Set gain incremented
	SetGain(CurrentGain);
}

//Decrements general volume
void SoundManager::VolumeDown(float amount)
{
    //Reduce volume in "LISTENER"
	
	//Local variables
	float CurrentGain;

	//Value to decrement
	CurrentGain = GetGain();
	//Decrement
	CurrentGain -= amount;
	//Limits
	if (CurrentGain < 0)
	{
		CurrentGain = 0;
	}
	//Set gain decremented
	SetGain(CurrentGain);
}

//Reset system
void SoundManager::Reset()
{
	Stop();
	_releaseALData();
}

//Pause sound playing
void SoundManager::Pause()
{
	ALint state;
	SourcesVector::iterator it;
	mPause = !mPause;
	//LOOP - For each source registerd
	for( it = mSources.begin(); it != mSources.end(); ++it )
	{
		alGetSourcei( (*it).source, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING && mPause)
		{
			alSourcePause((*it).source);
		}
		else if (state == AL_PAUSED && !mPause)
		{
			alSourcePlay((*it).source);
		}
	}//LOOP END

	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::Pause","Failure while pausing/playing source (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
}

//Stop sounds playing
void SoundManager::Stop()
{
	ALint state;
	SourcesVector::iterator it;

	//LOOP - For each source registered
	for( it = mSources.begin(); it != mSources.end(); ++it )
	{
		alGetSourcei((*it).source,AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING || state == AL_PAUSED)
		{
			alSourceStop((*it).source);
		}
	}//LOOP END

	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::Stop","Failure while stopping source (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}

	//Stop all streaming music
	StreamBuffersMap::iterator bufit;
	for(bufit = mStreamBuffers.begin(); bufit != mStreamBuffers.end(); bufit++)
	{
		//Call the STOP function
		bufit->second->Stop(false);
	}//LOOP END

	//Unmark all fadeouts
	mFadeOutSources.clear();
}

void SoundManager::Update ( float dt )
{
	//Makes periodic sound updates if necessary
	//LOOP - Update streams playing
	StreamBuffersMap::iterator bufit;
	for(bufit = mStreamBuffers.begin(); bufit != mStreamBuffers.end(); bufit++)
	{
		//Call the update function
		bufit->second->Update();
	}//LOOP END

	//-----Process "FadeOuts"-------------------------------
	float sourcegain;
	FadeOutSources::iterator itr=mFadeOutSources.begin();	
	//LOOP - Decrement sources gain
	while(itr!=mFadeOutSources.end())
	{
		//Decrement value of gain
		GetSourceGain((*itr).source,sourcegain);
		sourcegain -= dt * (*itr).rate;
		SetSourceGain((*itr).source,sourcegain);

		//Check if gain is small enough to erase from list
		//IF - Source gain is small enough
		if(sourcegain < 0.001f)
		{
			itr = mFadeOutSources.erase(itr);
		}//ELSE 
		else
		{
			++itr;
		}//IF
	}
	//LOOP END

	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::Update","Failure while Getting/Setting source gain(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	
	//-----Process dynamic buffers updates------------------
	SourcesVector::iterator srcitr;

	//LOOP - Check all sources
	for(srcitr = mSources.begin(); srcitr != mSources.end(); ++srcitr)
	{
		ALint state;
		//IF - Source state is not playing - Detach sound buffer from it
		alGetSourcei((*srcitr).source,AL_SOURCE_STATE, &state);
		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::Update","Failure while getting source state(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
		else
		{
			//IF - Was playing something  and stopped
			if (state != AL_PLAYING && state != AL_PAUSED && (*srcitr).actionplayed != "")		
			{
				_unbindSourceToBuffer(srcitr);
			}//IF	
		}
	}//LOOP END
}

//Play a sound - 3D sound (action) - AN ACTION IS NOT MODIFIED DURING PLAYING
bool SoundManager::PlayAction( const std::string& action, float posx, float posy, float posz)
{
	if (mSoundON)
	{	
		BuffersMap::iterator it = mBuffers.find(action);
		if (it==mBuffers.end())
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Action " + action + " not found",LOGEXCEPTION);
			return false;
		}
		IdBuffer idBuffer = (*it).second;
		int index;
		ALfloat Pos[3] = { posx, posy, posz }; //3d pos
		ALfloat Vel[3] = { 0.0, 0.0, 0.0 };
		
		index = _getSource(false);
	
		if (index==-1)
			return false;
	
		//Store last action played
		mSources[index].actionplayed = action;

		alSourcei (mSources[index].source, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcei (mSources[index].source, AL_BUFFER, idBuffer);
		alSourcef (mSources[index].source, AL_PITCH, 1.0);
		alSourcef (mSources[index].source, AL_GAIN, 1.0);
		alSourcefv(mSources[index].source, AL_POSITION, Pos);
		alSourcefv(mSources[index].source, AL_VELOCITY, Vel);
		alSourcei (mSources[index].source, AL_LOOPING, AL_FALSE);
	
		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Failure while Setting source attributes!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
		alSourcePlay(mSources[index].source);
		//Check errors
		if((error = alGetError() != AL_NO_ERROR))
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Failure while Playing source!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
	}
	return true;
}

//Play a sound - 2D sound (action) - AN ACTION IS NOT MODIFIED DURING PLAYING
bool SoundManager::PlayAction(const std::string& action)
{
	if (mSoundON)
	{	
		//Check if there is an action registered
		BuffersMap::iterator it = mBuffers.find(action);
		if (it==mBuffers.end())
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Action " + action + " not found",LOGEXCEPTION);
			return false;
		}
		IdBuffer idBuffer = (*it).second;
		int index;
		ALfloat vector[3] = { 0.0, 0.0, 0.0 }; //2D SOUND
			
		index = _getSource(false);
	
		if (index==-1)
			return false;
		
		//Store last action played
		mSources[index].actionplayed = action;
			
		alSourcei (mSources[index].source, AL_BUFFER, idBuffer);
		alSourcei (mSources[index].source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef (mSources[index].source, AL_PITCH, 1.0);
		alSourcef (mSources[index].source, AL_GAIN, 1.0);
		alSourcefv(mSources[index].source, AL_POSITION, vector);
		alSourcefv(mSources[index].source, AL_VELOCITY, vector);
		alSourcefv(mSources[index].source, AL_DIRECTION, vector);
		alSourcei (mSources[index].source, AL_LOOPING, AL_FALSE);
		
		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Failure while Setting source attributes!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}	

		alSourcePlay(mSources[index].source);

		//Check errors
		error = alGetError();
		if(error  != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayAction","Failure while playing source!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
	}
	return true;
}

//Create a source
unsigned int SoundManager::CreateSource()
{
	int index(_getSource(true));
	return mSources[index].source;
}

//Delete a source
bool SoundManager::DeleteSource(unsigned int source)
{
	//Find this source in vector
	SourcesVector::iterator srcitr;
	ALuint src = static_cast<ALuint>(source);
	//LOOP - Search requested source
	for(srcitr = mSources.begin(); srcitr != mSources.end();++srcitr)
	{
		if(src == (*srcitr).source)
		{
			break;
		}
	}//LOOP

	//IF - Found
	if(srcitr != mSources.end())
	{
		StopSource((*srcitr).source);
		(*srcitr).reserved = false;
		(*srcitr).actionplayed = "";
		return true;
	}//IF

	//Not found
	return false;
}

//Play a source relative (2d)
bool SoundManager::PlayRelativeSource(unsigned int source, const std::string& action, bool loop)
{
	if (mSoundON)
	{
		if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
		{
			ALboolean alloop;
	
			//Finf action to play
			if (mBuffers.find(action)==mBuffers.end())
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayRelativeSource","Action " + action + " not found",LOGEXCEPTION);
				return false;
			}

			//If it has to loop, and it is playing, dont do it again
			if (loop)
			{
				//Get state of source
				ALenum state;
				alGetSourcei(mSources[source].source, AL_SOURCE_STATE, &state);

				//Playing?
				if(state == AL_PLAYING && mSources[source].actionplayed == action)
					return true;  //Just return, playing with loop
			
				StopSource(source);
				alloop = AL_TRUE;

			}
			else
			{
				StopSource(source);
				alloop = AL_FALSE;
			}
			//Memorize what is it playing
			mSources[source].actionplayed = action;
				
			ALfloat vector[3] = { 0.0, 0.0, 0.0 };
			alSourcei (mSources[source].source, AL_BUFFER, mBuffers[action]);
			alSourcei (mSources[source].source, AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef (mSources[source].source, AL_PITCH, 1.0);
			alSourcef (mSources[source].source, AL_GAIN, 1.0);
			alSourcefv(mSources[source].source, AL_POSITION, vector);
			alSourcefv(mSources[source].source, AL_VELOCITY, vector);
			alSourcefv(mSources[source].source, AL_DIRECTION, vector);
			alSourcei (mSources[source].source, AL_LOOPING, alloop);

			//Check errors
			ALenum error(alGetError());
			if(error != AL_NO_ERROR)
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayRelativeSource","Failure while Setting source attributes!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
			}
			alSourcePlay(mSources[source].source);
			//Check errors
			if((error = alGetError() != AL_NO_ERROR))
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayRelativeSource","Failure while playing source!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
			}	
			return true;
		}	
		else
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayRelativeSource","Can't play source. Invalid source " + source,LOGEXCEPTION);
			return false;
		}
	}
	return true;
}

//Play a source 3D
bool SoundManager::PlaySource(unsigned int source, const std::string& action, bool loop)
{
	if (mSoundON)
	{
		if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
		{

			if (mBuffers.find(action)==mBuffers.end())
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlaySource","Action " + action + " not found",LOGEXCEPTION);
				return false;
			}
	
			ALboolean alloop;
			//If it has to loop, and it is playing, dont do it again
			if (loop)
			{
				//Get state of source
				ALenum state;
				alGetSourcei(mSources[source].source, AL_SOURCE_STATE, &state);

				//Playing?
				if(state == AL_PLAYING && mSources[source].actionplayed == action)
					return true;  //Just return, playing with loop
			
				StopSource(source);
				alloop = AL_TRUE;

			}
			else
			{
				StopSource(source);
				alloop = AL_FALSE;
			}

			//Memorize what is it playing
			mSources[source].actionplayed = action;

			alSourcei (mSources[source].source, AL_SOURCE_RELATIVE, AL_FALSE);
			alSourcei (mSources[source].source, AL_BUFFER, mBuffers[action]);
			alSourcef (mSources[source].source, AL_PITCH, 1.0);
			alSourcef (mSources[source].source, AL_GAIN, 1.0);
			alSourcei (mSources[source].source, AL_LOOPING, alloop);
			
			//Check errors
			ALenum error(alGetError());
			if(error != AL_NO_ERROR)
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlaySource","Failure while Setting source attributes!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
			}	
			alSourcePlay(mSources[source].source);
			if((error = alGetError() != AL_NO_ERROR))
			{
				SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlaySource","Failure while Playing source!(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
			}		
			return true;
		}	
		else
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlaySource","Can't play source. Invalid source " + source,LOGEXCEPTION);
			return false;
		}
	}
	return true;
}

//Pause a source
bool SoundManager::PauseSource(unsigned int source)
{
	if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
	{
		ALint state;
		
		alGetSourcei(mSources[source].source,AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING)
		{
			alSourcePause(mSources[source].source);
		}
		else if (state == AL_PAUSED)
		{
			alSourcePlay(mSources[source].source);
		}		

		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PauseSource","Failure while pausing/playing source (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}

		return true;
	}
	else
	{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PauseSource(","Can't pause source. Invalid source " + source,LOGEXCEPTION);
		return false;
	}
}

//Stop playing a source
bool SoundManager::StopSource(unsigned int source)
{
	if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
	{
		ALint state;
		
		alGetSourcei(mSources[source].source,AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING || state== AL_PAUSED)
		{
			alSourceStop(mSources[source].source);
		}

		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)	
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::StopSource","Failure while stopping source (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}

		//Check if there was a fadeout and clear it
		//LOOP - Search for source fading out
		FadeOutSources::iterator fadeit;
		bool found(false);
		//LOOP - Decrement sources gain
		for(fadeit=mFadeOutSources.begin();fadeit!=mFadeOutSources.end();++fadeit)
		{
			if((*fadeit).source == static_cast<ALuint>(source))
			{
				found = true;
				break;
			}
		}
		//LOOP END
		if(found)
			mFadeOutSources.erase(fadeit);

		return true;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::StopSource(","Can't stop source. Invalid source " + source,LOGEXCEPTION);
		return false;
	}
	
}

//Register a source for fadeout
void SoundManager::FadeOut ( unsigned int source , float time = 2.0)
{
	assert(time >0.5f);
	//Marks a source to make a fadeout
	mFadeOutSources.push_back(FadeOutInfo(source,time*1000,false));
}

//Set position of source (3d sound)
bool SoundManager::SetSourcePosition( unsigned int source, float posx, float posy, float posz)
{
	ALfloat pos[3] = { posx, posy, posz };	
	alSourcefv(mSources[source].source, AL_POSITION, pos);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetSourcePosition","Failure while setting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		return false;
	}
	return true;
}

//Get position of source (3d sound)
bool SoundManager::GetSourcePosition( unsigned int source, float& posx, float& posy, float& posz )
{
	ALfloat pos[3];
	alGetSourcefv(mSources[source].source, AL_POSITION, pos);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetSourcePosition","Failure while getting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		return false;
	}
	posx = pos[0];
	posy = pos[1];
	posz = pos[2];
	return true;
}

//Set speed of source (3d source)
bool SoundManager::SetSourceSpeed( unsigned int source, float velx, float vely, float velz )
{
	if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
	{
		ALfloat vel[3] = { velx, vely, velz };
		alSourcefv(mSources[source].source, AL_VELOCITY, vel);
		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetSourceSpeed","Failure while setting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
		return true;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetSourceSpeed","Can't get source speed. Invalid source " + source ,LOGEXCEPTION);
		return false;
	}
}

//Get speed of source (3d source)
bool SoundManager::GetSourceSpeed(unsigned int source, float& velx, float& vely, float& velz)
{
	if (source >= 0 && (unsigned int)source<mSources.size() && mSources[source].reserved)
	{
		ALfloat vel[3];
		alGetSourcefv(mSources[source].source, AL_VELOCITY, vel);
		//Check errors
		ALenum error(alGetError());
		if(error != AL_NO_ERROR)
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetSourceSpeed","Failure while getting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		}
		velx = vel[0];
		vely = vel[1];
		velz = vel[2];
		return true;
	}
	else
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetSourceSpeed","Can't set source speed. Invalid source " + source ,LOGEXCEPTION);
		return false;
	}	
}

//Set gain for source
bool SoundManager::SetSourceGain(unsigned int source, float gain)
{

	//Check value between boundaries
	float mingain,maxgain;
	alGetSourcef(mSources[source].source, AL_MIN_GAIN, &mingain);
	alGetSourcef(mSources[source].source, AL_MAX_GAIN, &maxgain);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetSourceGain","Failure while getting min and max gain values(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
		return false;
	}

	if(mingain > gain)
		gain = mingain;
	if(maxgain < gain)
		gain = maxgain;

	alSourcef(mSources[source].source, AL_GAIN, gain);
	//Check errors
	error = alGetError();
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetSourceGain","Failure while setting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	return true;

}

//Get gain for source
bool SoundManager::GetSourceGain(unsigned int source, float& gain)
{
	alGetSourcef(source, AL_GAIN, &gain);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetSourceGain","Failure while getting source values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	return true;
}

//Set listener position (3d sound)
void SoundManager::SetListenerPosition( float posx, float posy, float posz )
{
	alListener3f(AL_POSITION, posx,posy,posz);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetListenerPosition","Failure while setting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
}

//Get listener position (3d sound)
void SoundManager::GetListenerPosition(  float& posx, float& posy, float& posz)
{
	ALfloat pos[3];
	alGetListenerfv(AL_POSITION, pos);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetListenerPosition","Failure while getting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	posx = pos[0];
	posy = pos[1];
	posz = pos[2];
}

//Set listener Speed (3d sound)
void SoundManager::SetListenerSpeed( float velx, float vely, float velz )
{
	ALfloat vel[3] = { velx, vely, velz };
	alListenerfv(AL_VELOCITY, vel);	
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetListenerSpeed","Failure while setting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
}

//Get listener Speed (3d sound)
void SoundManager::GetListenerSpeed( float& velx, float& vely, float& velz )
{
	ALfloat vel[3];
	alGetListenerfv(AL_VELOCITY, vel);	
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetListenerSpeed","Failure while getting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	velx = vel[0];
	vely = vel[1];
	velz = vel[2];
}

//Set listener orientation (3d sound)
void SoundManager::SetListenerOrientation(	float lookatx, float lookaty, float lookatz,
											float upx, float upy, float upz )
{
	// VSpeed of the listener.
	ALfloat ListenerOri[6] = { lookatx, lookaty, lookatz, upx,upy, upz };
	alListenerfv(AL_ORIENTATION, ListenerOri);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::SetListenerOrientation","Failure while setting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
}

//Get listener orientation (3d sound
void SoundManager::GetListenerOrientation(	float& lookatx, float& lookaty, float& lookatz,
											float& upx, float& upy, float& upz )
{
	ALfloat ori[6];
	alGetListenerfv(AL_ORIENTATION, ori);	
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::GetListenerOrientation","Failure while setting listener values (" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	lookatx = ori[0];
	lookaty = ori[1];
	lookatz	= ori[2];
	upx	= ori[3];
	upy	= ori[4];
	upz	= ori[5];
}

//Play music
bool SoundManager::PlayMusic(const std::string &name, bool loop)	
{
	//Find action requested
	StreamBuffersMap::iterator it =  mStreamBuffers.find(name);
	//IF - Action to play not found
	if(it == mStreamBuffers.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayMusic","Stream sound '" + name + "' not found, and will not play" ,LOGEXCEPTION);
		return false;
	}

	//IF - Already playing
	if(
		it->second->IsPlaying()
		)
	{
		return true;
	}//ELSE - Not playing - so delete music object
	else 
	{
		//IF - Playback
		if(!it->second->Playback(loop))
		{
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::PlayMusic","Not possible to play stream music file",LOGEXCEPTION);
			return false;
		}
	}

	return true;
}

void SoundManager::FadeOutMusic(const std::string& name, float time = 2.0)
{
	assert(time >0.5f);
	//Find action requested
	StreamBuffersMap::iterator streamit =  mStreamBuffers.find(name);
	//IF - Action to play not found
	if(streamit == mStreamBuffers.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::FadeOutMusic","Unable to fadeout Stream music '" + name + "': It wasnt loaded before" ,LOGEXCEPTION);
		return;
	}//IF

	//Marks a source to make a fadeout
	ALuint source = streamit->second->GetSource();
	mFadeOutSources.push_back(FadeOutInfo(source,time*1000,true));

}

//Stop music
bool SoundManager::StopMusic(const std::string &name, bool restarttrack)	
{
	//Find action requested
	StreamBuffersMap::iterator streamit =  mStreamBuffers.find(name);
	//IF - Action to play not found
	if(streamit == mStreamBuffers.end())
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::StopMusic","Unable to stop Stream music '" + name + "': It wasnt loaded before" ,LOGEXCEPTION);
		return false;
	}

	//Stop
	(streamit->second->Stop(restarttrack));

	//Memorize source id
	ALuint source (streamit->second->GetSource());

	//Clear fadeout if there was one
	FadeOutSources::iterator fadeit;
	//LOOP - Decrement sources gain
	for(fadeit=mFadeOutSources.begin();fadeit!=mFadeOutSources.end();++fadeit)
	{
		if((*fadeit).source == static_cast<int>(source))  //UGLY SIGNED-UNSIGNED TO ADAPT TO STREAMS..
		{
			mFadeOutSources.erase(fadeit);
			break;
		}
	}
	//LOOP END

	return true;
}

std::string SoundManager::GetALErrorString(ALenum err, bool alcerror)
{
	//IF - A call to al...
	if(!alcerror)
	{
		switch(err)
		{
			case AL_NO_ERROR:
				return std::string("AL_NO_ERROR");
			break;

			case AL_INVALID_NAME:
				return std::string("AL_INVALID_NAME");
			break;

			case AL_INVALID_ENUM:
				return std::string("AL_INVALID_ENUM");
			break;

			case AL_INVALID_VALUE:
				return std::string("AL_INVALID_VALUE");
			break;

			case AL_INVALID_OPERATION:
				return std::string("AL_INVALID_OPERATION");
			break;

			case AL_OUT_OF_MEMORY:
				return std::string("AL_OUT_OF_MEMORY");
			break;
		};
	}//ELSE - A call to alc...
	else
	{
	switch(err)
		{
			case ALC_NO_ERROR:
				return std::string("ALC_NO_ERROR");
			break;

			case ALC_INVALID_DEVICE:
				return std::string("ALC_INVALID_DEVICE");
			break;

			case ALC_INVALID_CONTEXT:
				return std::string("ALC_INVALID_CONTEXT");
			break;

			case ALC_INVALID_ENUM:
				return std::string("ALC_INVALID_ENUM");
			break;

			case ALC_INVALID_VALUE:
				return std::string("ALC_INVALID_VALUE");
			break;

			case ALC_OUT_OF_MEMORY:
				return std::string("ALC_OUT_OF_MEMORY");
			break;
		};		
	}//IF


	return "";
}

//TODO: ENABLE OPTIONS FOR AUDIO
bool SoundManager::_initAL()
{
	SingletonLogMgr::Instance()->AddNewLine("SoundManager::InitAL","Initializing sound system...",LOGNORMAL);
	
	ALenum error;
	ALCdevice* pDevice;
	ALCcontext* pContext;

	SingletonLogMgr::Instance()->AddNewLine("SoundManager::InitAL","Opening default sound device...",LOGNORMAL);
	// Get handle to default device.
	pDevice = alcOpenDevice(NULL);

	if(pDevice == NULL)
		throw GenericException("Cant open default sound device. Reinstall drivers and check audio device configuration",GenericException::LIBRARY_ERROR);

	SingletonLogMgr::Instance()->AddNewLine("SoundManager::InitAL","Default sound device opened",LOGNORMAL);

	// Create audio context.
	pContext = alcCreateContext(pDevice, NULL);
	if(pContext == NULL)
		throw GenericException("Cant open default sound device. Reinstall drivers and check audio device configuration",GenericException::LIBRARY_ERROR);
	// Set active context.
	alcMakeContextCurrent(pContext);

	// Check for an error.
	if ((error=alcGetError(pDevice)) != ALC_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::InitAL","Can't create openAL context (" + GetALErrorString(error,true) + ")",LOGEXCEPTION);
		throw(GenericException("Can't create openAL context (" + GetALErrorString(error,true) + ")",GenericException::LIBRARY_ERROR));
	}

	SingletonLogMgr::Instance()->AddNewLine("SoundManager::InitAL","Sound system initialized correctly",LOGNORMAL);
	return true;
}

void SoundManager::_initResources()
{
	//Pre-allocate all available sources (no more no less)

	bool noerror(true);
	//LOOP - Allocate sources
	while(noerror && mSources.size() < (NUM_MAX_SOURCES - 2))  //Let's say 2 streaming objects at the same time... (not important in my game now... TODO)
	{
		//Generate new source
		InfoSource info;
		alGenSources(1,&info.source);
		ALenum error(alGetError());
		//IF - Error (sources pool depleted)
		if (error!=AL_NO_ERROR)
		{
			noerror = false;
		}
		else
		{
			info.reserved=false;
			mSources.push_back(info);
			std::stringstream ss;
			ss << "Source Created - Number of sources: "<< mSources.size();
			SingletonLogMgr::Instance()->AddNewLine("SoundManager::_getSource",ss.str(),LOGDEBUG);			
		}		
	}//LOOP END
}

//Perform cleanup
void SoundManager::_release()
{
	//Dynamic memory deallocation

	//Finishing of OpenAl
	if(mSoundON)
	{
		//Release
	  	_releaseAL();
		mSoundON = false;
	}
}

//Cleanup of OPENAL system
void SoundManager::_releaseAL()
{
	bool success = true;

	SingletonLogMgr::Instance()->AddNewLine("SoundManager::releaseAL","Releasing sound system..",LOGNORMAL);
	
	//Clear all data
	_releaseALData();
	
	ALCcontext* pCurContext;
	ALCdevice* pCurDevice;

	// Get the current context.
	pCurContext = alcGetCurrentContext();

	// Get the device used by that context.
	pCurDevice = alcGetContextsDevice(pCurContext);

	// Reset the current context to NULL.
	if(!alcMakeContextCurrent(NULL))
		success = false;

	// Release the context and the device.
	alcDestroyContext(pCurContext);
	
	if(!alcCloseDevice(pCurDevice))
		success = false;
	
	if(!success)
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::releaseAL","Sound system released INCORRECLY",LOGEXCEPTION);
	else
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::releaseAL","Sound system released correctly",LOGNORMAL);
	
}

bool SoundManager::_loadSound(const std::string& file, IdBuffer& buffer)
{
	// Variables to load into.
	FILE *fd;
	ALenum format;
	ALenum error;
	ALsizei size;
	ALvoid* data;
	ALsizei freq;
	ALboolean loop;

	// Load wav data into buffers.
	alGenBuffers(1, &buffer);

	if((error=alGetError()) != AL_NO_ERROR)
	{	
		alDeleteBuffers(1,&buffer);
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_loadSound","Error: Can't create openAL Buffer (" + GetALErrorString(error,false)  + ")",LOGEXCEPTION);	
		return false;	
	}

	// Check if the file exists
	if ((fd = fopen(file.c_str(),"r"))==NULL)
	{
		alDeleteBuffers(1,&buffer);
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_loadSound","Error: Can't open file " + file,LOGEXCEPTION);			
		return false;
	}
	else
	{
		fclose(fd);
	}

	alutLoadWAVFile((ALbyte*)file.c_str(), &format, &data, &size, &freq, &loop);
	alBufferData(buffer, format, data, size, freq);
	alutUnloadWAV(format, data, size, freq);
	if ((error=alGetError()) != AL_NO_ERROR)
	{		
		alDeleteBuffers(1,&buffer);
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_loadSound","Error: Can't load sound file " + file + " (" + GetALErrorString(error,false)  + ")",LOGEXCEPTION);			
		return false;
	}		
	return true;
}

bool SoundManager::_deleteSound(IdBuffer buffer)
{
	//Delete the buffer - it can fail for various reasons
	alDeleteBuffers(1,&buffer);

	//Check error
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{	
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_deleteSound","Error: Can't delete openAL Buffer (" + GetALErrorString(error,false)  + ")",LOGEXCEPTION);	
		return false;	
	}

	return true;
}

bool SoundManager::_deleteSource(ALuint sourceid)
{

	//Delete the buffer - it can fail for various reasons
	alDeleteSources(1,&sourceid);

	//Check error
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{	
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_deleteSource","Error: Can't delete openAL Source (" + GetALErrorString(error,false)  + ")",LOGEXCEPTION);	
		return false;	
	}

	return true;
}
void SoundManager::_releaseALData()
{
	// Delete AL objets
	SourcesVector::iterator itSource;
	for( itSource = mSources.begin(); itSource != mSources.end(); ++itSource )
	{
		_deleteSource((*itSource).source);	
	}
	mSources.clear();

	BuffersMap::iterator itBuffer;
	for( itBuffer = mBuffers.begin(); itBuffer != mBuffers.end(); ++itBuffer )
	{
		_deleteSound((*itBuffer).second);
	}
	mBuffers.clear();

	StreamBuffersMap::iterator itstreams;
	for( itstreams = mStreamBuffers.begin(); itstreams != mStreamBuffers.end(); ++itstreams )
	{
		delete (*itstreams).second;
	}
	mStreamBuffers.clear();
}

int SoundManager::_getSource(bool reserved)
{
	// Find a unused source
	ALint state;
	
	//LOOP - Search in container
	for( unsigned int i=0; i < mSources.size(); i++)
	{
		//If - Not reserved
		if (!mSources[i].reserved)
		{
			// Check if it is being used
			alGetSourcei(mSources[i].source,AL_SOURCE_STATE, &state);
			if (state != AL_PLAYING && state != AL_PAUSED)
			{
				mSources[i].reserved = reserved;
				return i;
			}			
		}
	}//LOOP
		
	//All sources are used
	return -1;
}


void SoundManager::_unbindSourceToBuffer(SourcesVector::iterator srcitr)
{
	
	//DEBUG TESTS
	//int numbuffers(0);
	//alGetSourcei((*srcitr).source,AL_BUFFERS_PROCESSED,&numbuffers);
	//alSourceUnqueueBuffers((*srcitr).source,1,&buffid);

	int buffid(0);  //0 is reserved to unbind (NULL buffer)
	alGetSourcei((*srcitr).source,AL_BUFFER,&buffid);

	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_unbindSourceToBuffer","Failure while unbinding source to buffer(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	(*srcitr).actionplayed = "";	
}

void SoundManager::_unbindSourceToBuffer(int index)
{
	assert ((unsigned int)index < mSources.size());
	int buffid(0); //0 is reserved to unbind (NULL buffer)
	alGetSourcei(mSources[index].source,AL_BUFFER,&buffid);
	//Check errors
	ALenum error(alGetError());
	if(error != AL_NO_ERROR)
	{
		SingletonLogMgr::Instance()->AddNewLine("SoundManager::_unbindSourceToBuffer","Failure while unbinding source to buffer(" + GetALErrorString(error,false) + ")",LOGEXCEPTION);
	}
	mSources[index].actionplayed = "";	
}

//Gets a source playing this action id (-1 if none)
int SoundManager::_getSourcePlayingAction(const std::string& actionid, ALuint& outsourceid)
{
	SourcesVector::iterator srcitr;
	int i;
	//LOOP - Search in Sources Vector
	for(srcitr = mSources.begin(), i = 0; srcitr != mSources.end();++srcitr,i++)
	{
		//IF - Found
		if((*srcitr).actionplayed == actionid)
		{
			outsourceid = (*srcitr).source;
			return i;
		}//IF
	}//LOOP

	return -1;
}