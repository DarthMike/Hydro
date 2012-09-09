/*
	Filename: Singleton_Template.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Templates to give SINGLETON functionality to any given class
	Comments: 2 options: Meyers singleton or Phoenix Singleton
	Attribution: Based in article by J. Nakamura at http://www.devarticles.com/c/a/Cplusplus/
				 and book "Modern C++ Design" by Alexandrescu
				 TODO: STILL THIS TEMPLATES DONT OFFER DELETION AUTOMATICALLY! FOR NOW IS NOT A PROBLEM
				 AS I WILL USE THEM, BUT AUTOMATIC DELETION OF POINTER ON HEAP NEEDS TO BE PROVIDED
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/* CHANGELOG
	 - Added PHOENIX singleton implementation
	 - Corrected mistake: I switched Singleton to reference, and then back to pointer. I consider better the pointer
	 implementation, because you have to EXPLICITILY call "delete" on singleton instance (quite stupid). On the other
	 hand, if a reference is passed, and you create a temporal reference variable (in a function for example), THE 
	 OBJECT ITSELF IS DESTROYED WHEN FUNCTION GOES OUT OF SCOPE! Nasty nasty bug if you dont remember to put the & to the
	 temporal variable.... (it is easier to find a "Delete" than to realize where a variable goes out of scope...
     - Creation of Singleton is by heap allocation (new-delete). That means that till no Instance is called, object is 
	   not created. That also means, that a controlled destroyed must be called not to leak... For now, an implementation
	   of a singleton destroyer is not made, it is the next TODO.
     - Added methods "Destroy" to control when instance is destroyed: IT MUST BE CALLED BEFORE PROGRAM END!
*/

#ifndef _SINGLETON_TEMPLATES
#define _SINGLETON_TEMPLATES

#include <exception>
#include <assert.h>

//-----------IMPLEMENTATION OF MEYERS SINGLETON TEMPLATE--------------------
//MEYERS: NO ORDER OF DESTRUCTION IS GIVEN! - DESTROY MANUALLY!
template <class T>
class MeyersSingleton
{
public:
	//Main way to access class pointer and instance is by this function
	//(INSTANCE)
	inline static T* Instance() 
	{
		if(!mpInstance)
		{
			//Check singleton wasnt destroyed
			if(!mDestroyed)
			{
				_create();
			}
			else //DESTROYED!
			{
				//Throw runtime error!
				throw std::exception("Static dead reference encountered");
			}	
		}

		return mpInstance;
	}

	//Main way to destroy this singleton (AND YOU MUST!)
	static void Destroy()
	{
		if(mpInstance)
		{
			delete mpInstance;
			mpInstance = NULL;
			mDestroyed =  true;	//Track destruction of object
		}
	}
protected:
	//----- TEMPLATE SINGLETON STRUCTURE -------
	//Constructor, copy constructor, destructor, and operator = hidden
	MeyersSingleton(){}
	~MeyersSingleton()
	{
		mpInstance = NULL;
		mDestroyed =  true;	//Track destruction of object
	}
	MeyersSingleton(MeyersSingleton const&){}		
	MeyersSingleton& operator=(MeyersSingleton const&){}  // assign op hidden
	//Internal trackings
	static T* mpInstance;
	static bool mDestroyed;
	//Singleton creation function
	static void _create()
	{	
		assert(!mpInstance);
		mpInstance = new T;
	}
};

//Definition of static variable
template <class T> T* MeyersSingleton<T>::mpInstance = NULL;
template <class T> bool MeyersSingleton<T>::mDestroyed = false;

//---------------IMPLEMENTATION OF PHOENIX SINGLETON TEMPLATE------------------
//PHOENIX: WHEN IT IS DESTROYED AND CALLED AGAIN... IT REVIVES FROM ASHES, LIKE
//THE PHOENIX ;) ONLY TO BE DESTROYED AGAIN ATEXIT OF PROGRAM  - DESTROY MANUALLY!
template <class T>
class PhoenixSingleton
{
public:
	//Main way to access class pointer and instance is by this function
	//(INSTANCE)
	inline static T* Instance() 
	{
		if(!mpInstance)
		{
			//Check singleton wasnt destroyed
			if(!mDestroyed)
			{
				_create();
			}
			else //DESTROYED!
			{
				//Phoenix singleton... revive!
				_revive();
			}	

		}

		return mpInstance;
	}

	//Main way to destroy this singleton (AND YOU MUST!)	
	static void Destroy()
	{
		if(mpInstance)
		{
			delete mpInstance;
			mpInstance = NULL;
			mDestroyed =  true;	//Track destruction of object
		}
	}
protected:
	//----- TEMPLATE SINGLETON STRUCTURE -------
	//Constructor, copy constructor, destructor, and operator = hidden
	PhoenixSingleton()
	{}
	~PhoenixSingleton()
	{
		mpInstance = NULL;
		mDestroyed =  true;	//Track destruction of object
	}
	PhoenixSingleton(PhoenixSingleton const&)
	{}
	PhoenixSingleton& operator=(PhoenixSingleton const&)  // assign op hidden
	{}
	//The only instance
	static T* mpInstance;
	//Internal trackings
	static bool mDestroyed;
	//Singleton creation function
	static void _create()
	{	
		assert(!mpInstance);
		mpInstance = new T;
	}
	//Revive and Kill functions
	static void _revive()
	{
		//Create again the singleton
		_create();
		
		//Mow we have full pointer and again resurrected singleton
		//At exit of program, we have to delete this singleton manually:

		#ifdef	ATEXIT_FIXED
			atexit(_kill);
		#endif
	}
	static void _kill()
	{
		//Just call manually the destructor
		mpInstance->~PhoenixSingleton();
	}
};

//Definition of static variable
template <class T> T* PhoenixSingleton<T>::mpInstance = NULL;
template <class T> bool PhoenixSingleton<T>::mDestroyed = false;

#endif