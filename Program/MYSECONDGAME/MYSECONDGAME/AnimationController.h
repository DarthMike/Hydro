/*
	Filename: AnimationController.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate control of an entity animation in a state by state basis
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#ifndef _ANIMATIONCONTROLLER
#define _ANIMATIONCONTROLLER

//Library dependencies
#include "boost\shared_ptr.hpp"  //Smart pointer facilites - not using std because it cant go inside an 
								 //STL container as we do in event manager...
#include <queue>
//Class dependencies
#include "Shared_Resources.h"

//Definitions
class AnimationController;
//A smart pointer to hold the created blob controller
typedef boost::shared_ptr<AnimationController> AnimationControllerPointer;

class AnimationController
{
protected:
	typedef struct ControlInfo //Animation control info
	{
		//Constructor
		ControlInfo(bool loop, int animation,bool freeze):
		loop(loop),
		animation(animation),
		freezeatend(freeze),
		isactive(false)
		{}
		//Operator = to ease copies
		ControlInfo& operator=(const ControlInfo &newpars)
		{
			//Deep copy of elements in struct
			loop = newpars.loop;
			animation =  newpars.animation;
			freezeatend = newpars.freezeatend;
			return (*this);
		}

		bool loop;
		int animation;
		bool freezeatend;
		bool isactive;
	}ControlInfo;
	
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	AnimationController(SpritePointer tocontrol, int defaultanim, bool defaultlooping = true, bool defaultfreeze = false):
		mSprite(tocontrol),
		mCurrentAnim(defaultlooping,defaultanim,defaultfreeze),
		mDefaultAnim(defaultlooping,defaultanim,defaultfreeze),
		mNextAnim(defaultlooping,defaultanim,defaultfreeze),
		mTimeFilter(50.0f),
		mCounter(0.0f)
	{	
		_init();
	}
	virtual ~AnimationController()
	{
		_release();
	}

	//----- GET/SET FUNCTIONS -----
	void SetTimeFilter(float newtime) { mTimeFilter = newtime; }
	//----- OTHER FUNCTIONS -----
	void Update(float dt);				//Update method
	void SetNextAnimation(bool usefilter,int number,bool freeze);	//Set next animation (with time filtering as option)
	void ForceDefaultAnim(bool usefilter);				//Force the default animation (with time filtering as option)
	bool IsCurrentAnimationFinished();  //Check if current animation is finished
	bool IsCurrentAnimationLooping();	//Chekc if current animation is looping
	bool IsAnimationStoppedAtEnd();		//Check if animation is stopped at end frame
	bool IsAnimActive(int number);		//Check the animation active
	bool IsCurrentAnim(int number);		//Check if current animation name is this one
	//----- PUBLIC VARIABLES ------

protected:
	//----- INTERNAL VARIABLES -----
	SpritePointer mSprite;		//The controlled sprite
	ControlInfo mDefaultAnim;			//The default animation to display
	ControlInfo mCurrentAnim;			//Current displaying animation
	ControlInfo mNextAnim;				//Animation to change to
	float mCounter;						//Timing variable
	float mTimeFilter;					//Time filter to change animations
	//----- INTERNAL FUNCTIONS -----
	void _init();
	void _release();
	void _restartAnimation(int id);
};

#endif