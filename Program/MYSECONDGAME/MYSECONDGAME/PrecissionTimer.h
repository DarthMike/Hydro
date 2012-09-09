/*
	Filename: PrecissionTimer.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a precission timer (if hardware permits it)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#ifndef _PRECISSIONTIMER
#define _PRECISSIONTIMER

//Library dependencies
#include <windows.h>   //Queryperformance counter 
//Class dependencies
#include "IndieLib\Common\LibHeaders\Indie.h"  //Default if not available

class PrecissionTimer
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	PrecissionTimer():
    mHighRes(false)
	{
		_init();  
	}
	virtual ~PrecissionTimer()
	{
	}
	//----- GET/SET FUNCTIONS -----
	float GetTicks();
	//----- OTHER FUNCTIONS -----
    void Start();
    void Stop();
	//----- PUBLIC VARIABLES ------

private:
	//----- INTERNAL VARIABLES -----
	bool mHighRes;    //Using High-res timer?
	IND_Timer mLowResTimer;	//Low-res timer

	__int64 mStartTime;	//Start time(Windows- specific format...)
	__int64 mFinalTime;	//End time(Windows- specific format...)
	__int64 mElapsedTime;  //Elapsed time (Windows- specific format...)
	__int64 mFrequency;	//High-res timer frequency
	//----- INTERNAL FUNCTIONS -----
	void _init();
};

#endif