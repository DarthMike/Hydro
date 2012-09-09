/*
	Filename: AnimationController.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate control of an entity animation in a state by state basis
	Comments: It depends of IndieLib
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

#include "AnimationController.h"
#include "IndieLibManager.h"
#include "GameEvents.h"

//Update method
void AnimationController::Update(float dt)	
{
	mCounter +=dt;
	//Control of new animation to display
	//IF - Change animation
	if(mCurrentAnim.animation != mNextAnim.animation)
	{
		mSprite->SetSequence(mNextAnim.animation);
		if(mNextAnim.loop)
			mSprite->SetNumReplays(-1);
		else
			mSprite->SetNumReplays(0);

		mCurrentAnim = mNextAnim;
		mCurrentAnim.isactive = true;
		mCounter = 0.0f;
	}//ELSE - Animation ended
	else if(!mCurrentAnim.freezeatend
		    &&
			!mSprite->GetAnimation()->GetIsActive(mCurrentAnim.animation))
	{
		if(mCurrentAnim.animation != mDefaultAnim.animation)
		{
			mSprite->SetSequence(mDefaultAnim.animation);
			if(mDefaultAnim.loop)
				mSprite->SetNumReplays(-1);
			else
				mSprite->SetNumReplays(0);
			mCurrentAnim = mNextAnim = mDefaultAnim;
		}
		mCurrentAnim.isactive = false;
		mCounter = 0.0f;
	}//ELSE - Animation is frozen at end
	else if(mCurrentAnim.freezeatend
		    &&
			!mSprite->GetAnimation()->GetIsActive(mCurrentAnim.animation))

	{
		mCurrentAnim.isactive = false;
	}
}

//Set next animation (to play when the other is finished)
void AnimationController::SetNextAnimation(bool usefilter,int number, bool freeze)
{
	if(mCurrentAnim.animation != number)
	{
		if((usefilter && mCounter >= mTimeFilter)
		   ||
		   !usefilter)
		{
			ControlInfo newanim(false,number,freeze);
			mNextAnim = newanim;
			//TODO: TAKE OUT THIS FUNCTION 
			_restartAnimation(mNextAnim.animation);  //Restart (it remembers where it was last)
		}
	}	
}

//Force the default animation
void AnimationController::ForceDefaultAnim(bool usefilter)
{
	if(mCurrentAnim.animation != mDefaultAnim.animation)
	{
		if((usefilter && mCounter >= mTimeFilter)
		    ||
			!usefilter)
		{
			mNextAnim = mDefaultAnim;
		}
	}
}

//Check the animation active
bool AnimationController::IsAnimActive(int number)
{
	return (mSprite->GetAnimation()->GetIsActive(number)
		    &&
			mCurrentAnim.animation == number);
}

//Check if current animation name is this one
bool AnimationController::IsCurrentAnim(int number)
{
	return (mCurrentAnim.animation == number);
}

//Check if current animation is finished	
bool AnimationController::IsCurrentAnimationFinished()  
{	
	bool isactive = mSprite->GetAnimation()->GetIsActive(mCurrentAnim.animation);
	return (
		    (!isactive  //Animation frame finished at the last
			 &&
			 !mCurrentAnim.isactive) //Tracking of finished
			 ||  
			 (mCurrentAnim.animation == mDefaultAnim.animation  //Default animation doesnt count if it loops (always can be overriden)
			  &&
			  mDefaultAnim.loop)
		   );
}

//Check if animation is looping
bool AnimationController::IsCurrentAnimationLooping()
{
	return (mCurrentAnim.loop);
}

bool AnimationController::IsAnimationStoppedAtEnd()
{
	//bool animactive = mSprite->GetAnimation()->GetIsActive(mCurrentAnim.animation);
	return(mCurrentAnim.freezeatend
		   &&
		   mSprite->GetAnimation()->GetActualFramePos(mCurrentAnim.animation) == (mSprite->GetAnimation()->GetNumFrames(mCurrentAnim.animation) - 1));
}
void AnimationController::_init()
{
	assert(mSprite->GetAnimation() != NULL);
	mSprite->SetSequence(mCurrentAnim.animation);  //Set the first animation
	if(mCurrentAnim.loop)
		mSprite->SetNumReplays(-1);
	else
		mSprite->SetNumReplays(0);
	mCurrentAnim.isactive = true;
}
	
void AnimationController::_release()
{
	//Stop all animations
	SingletonIndieLib::Instance()->Entity2dManager->Delete(mSprite.get());
}

void AnimationController::_restartAnimation(int id)
{
	if(mSprite->GetAnimation()->GetActualFramePos(id) != 0)
	{
		mSprite->GetAnimation()->SetActualFramePos(id,0);
	}
}