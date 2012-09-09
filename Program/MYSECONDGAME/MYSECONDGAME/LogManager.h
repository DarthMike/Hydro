/*
	Filename: LogManager.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Logging class for application
	Comments: 3 modes: Debug, normal, failure; needs of singleton template file
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/

/*
CHANGELOG:
	-Bug in enum LoggingPriority!  DEBUG IS NOT A GOOD NAME (THERE IS A DEFFINITION OF DEBUG IN MVC
	CHANGED TO LOGDEBUG, LOGEXCEPTION,LOGNORMAL
	- Changed open mode of file to not append but to delete contents (in release mode)/ if not file gets bigger and bigger and
	there is no sense to track all executions of game
	-Changed ctime to ctime_s to take out warning. Old code commented just in case...
*/

/*
TODO:
	- Manage writing periodically of messages in disk , as if there are too many loggines per second it is a performance killer.
	- Manage writing of repeated messages more than once, or in a number of times, just to increase performance in case of errors
	- Manage size of file. When it exceeds size, close it, delete it, or write a new one... something. Now it grows to infinite, 
	  in one play session (in RELEASE) mode	
*/

#ifndef _LOGMANAGER
#define _LOGMANAGER
//Library dependencies
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <time.h>
//Class dependencies
#include "Singleton_Template.h"

//General definitions
/* 
   DEBUG - ONLY WHEN LOGGING PRIORITY IS DEBUG, IT WILL BE SHOWN
   NORMAL - ALLWAYS SHOWN 
   EXCEPTION - ALLWAYS SHOWN WITH A SPECIAL ICON SHOWING ITS AN ERROR
*/
typedef enum LoggingPriority{LOGDEBUG = 0,LOGNORMAL=1,LOGEXCEPTION=2}LoggingPriority;

//DEBUG MODE ONLY WHEN IN DEBUG RELEASE OF VS2008
#ifdef _DEBUG
	#define LOGGING LOGDEBUG
#else
	#ifdef _DEBUGGING
	#define LOGGING LOGDEBUG
	#else
	#define LOGGING LOGNORMAL
	#endif
#endif

class LogManager : public PhoenixSingleton<LogManager>
{
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	LogManager(): mFileOpened(false), mPriority(LOGGING)
	{
		OpenLogFile(); //Open file at construction
	}
	~LogManager()
	{
		if(mFileOpened)
		{
			_InsertFinalText();
			CloseLogFile(); //Close file at deconstruction
		}
	}
	//----- GET/SET FUNCTIONS -----
	//----- OTHER FUNCTIONS -----
	void AddNewLine (const std::string& functioncall, const std::string& line, LoggingPriority p); //To add a new line to log
	bool OpenLogFile();									//Opens file explicitly (if not open previously)
	bool CloseLogFile();								//Closes file explicitly
	//----- PUBLIC VARIABLES ------	

protected:
	//----- INTERNAL VARIABLES -----
	static const std::string mFileName;					//Filename (constant) of log file
	char mTimeText[200];							    //Internal text string for use with ctime_s
	std::ofstream mFile;								//File stream
	bool mFileOpened;									//Open/close boolean control
	LoggingPriority mPriority;							//Which logs will be really stored
	//----- INTERNAL FUNCTIONS -----
	void _InsertInitialText();												//Initial text
	void _InsertFinalText();												//Final text
	void _AddLine(const std::string& line);									//Internal adding line to log
	void _GetPriorityMark(LoggingPriority p,std::string &text);	//Convert priority to a text mark
};

//Definitions - SINGLETON LOG MANAGER OBJECT
typedef LogManager SingletonLogMgr;

#endif
