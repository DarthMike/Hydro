/*
	Filename: SoundManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class which encapsulates managing of sound system
	Comments: ONLY FOR OPENAL / to do a wrapper independent of library
	Attribution:
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/* CHANGELOG
	- No more method for Releasing externally (Release()). It is done at object destruction only!
	- Method LoadSounds doesnt exist anymore / sounds must be commanded to load from resource manager
	- Added Method to delete sound buffer "DeleteSound". It will fail if sound is assigned to source
	  so it returns boolean, but throws no exception, only logs problem
	- Added Methods to load soundstream (music in old version) and delete soundstream
	- More than one music file can be opened and played at the same time, mMusic no longer exists, created
	another container of OggSreams to have all musics playing at the same time
	- Added class sound, to encapsulate a sound resource. For now it only has a name and if it is sound stream (music)
	or sound not streamed (fx)
	- Solved some bugs when deleting buffers (need to check if it is attached to any source first!!!)
	- Sound streaming problems solved
	- Followed recommendations found in the net about twisted do's and do not's with the library (static sources allocation....etc)
*/

#ifndef _SOUNDMANAGER
#define _SOUNDMANAGER

//Library dependencies
#include <string>
#include <map>
#include <queue>
#include <vector>
#include <list>
#include "al.h"   //OPENAL

//Classes dependencies
#include "Singleton_Template.h"
#include "GenericException.h"
#include "LogManager.h" 
#include "SoundDefs.h"
#include "OggStream.h"

//Definitions
const float SENSCAMBIOVOL = 1.3f;    //When change of volume, sensibility for each change
const float GAINLIMIT = 1.0f;		 //GAIN limit for source playing
const int NUM_MAX_SOURCES = 24;        //Simulatenous "Channels"
//Sound Resource
class SoundResource
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SoundResource(const std::string& name, bool isstream):
	  mName(name),
	  mIsStream(isstream)
	{}
	~SoundResource()
	{}
	//----- GET/SET FUNCTIONS -----
	//Query if it is a stream
	bool IsStream() const{ return mIsStream; }
	const std::string& GetName()const { return mName; }
private:
	//----- INTERNAL VARIABLES -----
	std::string mName;	//The name
	bool mIsStream;		//If it is a stream
	//----- INTERNAL FUNCTIONS -----
};

class SoundManager : public MeyersSingleton<SoundManager>
{
	//Definitions
public:
	typedef struct InfoSource
	{
		InfoSource():reserved(false),actionplayed("")
		{}
		ALuint	source;				//ID of source
		bool	reserved;			//Is it reserved
		std::string actionplayed;	//Action assigned id
	}InfoSource;  //Source info
	
	typedef struct FadeOutInfo		
	{
		FadeOutInfo(ALuint source, float fadetime, bool stream): 
		source(source),
		fadetime(fadetime),
		stream(stream),
		rate(GAINLIMIT/fadetime){}
		ALuint		source;
		float fadetime;
		float rate;
		bool stream;
	}FadeOutInfo;		//Info for sources reserved to make a "fadeout"	

	typedef std::string	Action;	//Name of action to play
	typedef ALuint	IdBuffer;	//BufferID for an action
//Definitions - Maps of contained elements
public:
	typedef std::map<Action, IdBuffer> BuffersMap;
	typedef std::map<Action, OggStream*> StreamBuffersMap;
	typedef std::vector<InfoSource> SourcesVector;
	typedef std::list<FadeOutInfo> FadeOutSources;
public:		
	//----- CONSTRUCTORS/DESTRUCTORS -----
	SoundManager(): mSoundON(true), mPause(false)
	{
		//Init system
		mSoundON = _initAL();
		_initResources();
	}
	~SoundManager()
	{
		//Release resources if not made previously
		_release();
	}
	//----- GET/SET FUNCTIONS -----
	void SetGain( float gain );		//Get General gain
	float GetGain();				//Set General gain
	std::string GetALErrorString(ALenum err, bool alcerror);
	
	//SOURCE DATA
	bool SetSourceGain		( unsigned int source, float inGain );	
	bool GetSourceGain		( unsigned int source, float& outGain );	
	bool SetSourcePosition	( unsigned int source, float posx, float posy, float posz);	
	bool GetSourcePosition	( unsigned int source, float& posx, float& posy, float& posz);
	bool SetSourceSpeed	( unsigned int source, float velx, float vely, float velz);
	bool GetSourceSpeed	( unsigned int source, float& velx, float& vely, float& velz);

	//LISTENER DATA
	void SetListenerPosition		( float posx, float posy, float posz);
	void GetListenerPosition		( float& posx, float& posy, float& posz);
	void SetListenerSpeed		( float velx, float vely, float velz);
	void GetListenerSpeed		( float& velx, float& vely, float& velz);
	void SetListenerOrientation	(	float lookatx, float lookaty, float lookatz,
											float upx, float upy, float upz );
	void GetListenerOrientation	(	float& lookatx, float& lookaty, float& lookatz,
											float& upx, float& upy, float& upz );
		
	//----- OTHER FUNCTIONS -----
	bool LoadSound( const std::string& file, const std::string& actionName );
	bool DeleteSound( const std::string& actionName );
	bool LoadSoundStream( const std::string& file, const std::string& name );
	bool DeleteSoundStream( const std::string& name );
	void Reset();
	void Pause();
	void Stop();
	void SoundOn() {mSoundON=true;}
	void SoundOff() {mSoundON=false;}
	void VolumeUp(float amount);
	void VolumeDown(float amount);     
	void FadeOut ( unsigned int source , float time);
	void FadeOutMusic(const std::string& name, float time);
	void Update(float dt);

	//ACTION FUNCTIONS - Sounds which are not modified when played, and not streamed
	bool	PlayAction	( const std::string& action);  //Without position - 2D - ACTION IS A SOUND WHICH WILL NOT BE MODIFIED WHILE PLAYED
	bool	PlayAction	( const std::string& action, float posx, float posy, float posz); //Position - 3D - ACTION IS A SOUND WHICH WILL NOT BE MODIFIED WHILE PLAYED

	//MUSIC(SOUND STREAMS) FUNCTIONS - Sounds which are not modified while played, and are streamed - MUSIC IS 2D
	bool PlayMusic (const std::string &name,  bool loop = true);		//Play music
	bool StopMusic (const std::string &name,  bool restarttrack);		//Stop music
		
	//SOURCE FUNCTIONS - To play any buffer data, using respective PlaySource or PlayRelativeSource. Attributes can be modified while playing
	unsigned int CreateSource		( void );
	bool DeleteSource		( unsigned int source );
    bool PlaySource			( unsigned int source, const std::string& action, bool loop ); //Play source in its position (3d sound)
	bool PlayRelativeSource	( unsigned int source, const std::string& action, bool loop ); //Play a source relative (2d sound)
	bool PauseSource		( unsigned int source );
	bool StopSource			( unsigned int source );

	//----- PUBLIC VARIABLES -----
	
protected:
	//----- INTERNAL VARIABLES -----
	bool	mSoundON;
	bool	mPause;
	BuffersMap			mBuffers;
	StreamBuffersMap	mStreamBuffers;
	SourcesVector		mSources;
	FadeOutSources		mFadeOutSources;
	
	//----- INTERNAL FUNCTIONS -----
	bool _initAL();
	void _initResources();
	void _release();
	void _releaseAL();
	bool _loadSound	(const std::string& file, IdBuffer& buffer );
	bool _deleteSound(IdBuffer buffer);
	bool _deleteSource(ALuint sourceid);
	void _releaseALData();
	int	_getSource(bool reserved=false );
	int SoundManager::_getSourcePlayingAction(const std::string& actionid, ALuint& outsourceid);
	void _unbindSourceToBuffer(SourcesVector::iterator srcitr);	
	void _unbindSourceToBuffer(int index);
};

//Singleton typedef
typedef MeyersSingleton<SoundManager> SingletonSoundMgr;

#endif
