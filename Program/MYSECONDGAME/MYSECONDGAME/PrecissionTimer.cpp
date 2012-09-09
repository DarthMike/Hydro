/*
	Filename: PrecissionTimer.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate a precission timer (if hardware permits it)
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "PrecissionTimer.h"

//Get ticks (in float)
float PrecissionTimer::GetTicks()
{
	if(mHighRes)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&mFinalTime);
		mElapsedTime = (mFinalTime - mStartTime) * 1000 / mFrequency;
		return(static_cast<float>(mElapsedTime));
	}
	else
		return(static_cast<float>(mLowResTimer.GetTicks()));
}

//Start count
void PrecissionTimer::Start()
{
	if(mHighRes)
		QueryPerformanceCounter((LARGE_INTEGER*)&mStartTime);
	else
		mLowResTimer.Start();
}

//Stop count
void PrecissionTimer::Stop()
{
	if(mHighRes)
		mElapsedTime = 0;
	else
		mLowResTimer.Stop();
}
	
void PrecissionTimer::_init()
{
	//Check if hardware permits precission timer
	if(QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency))
	{
		mHighRes = true;	
	}
}