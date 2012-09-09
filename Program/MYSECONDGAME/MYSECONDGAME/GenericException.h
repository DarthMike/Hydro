/*
	Filename: GeneralException.h
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate generic exceptions in program - derived from std::exception, so it
				 contains handling to generic library and memory allocation failures
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/
#ifndef _GENERICEXCEPTION
#define _GENERICEXCEPTION

//Library dependencies
#include <string>

//Class dependencies
#include <exception>

class GenericException : public std::exception
{
public:
	//Type definitions
	enum ExceptionCode{
            INVALIDPARAMS,
            GRAPHICS_ERROR,
            ITEM_NOT_FOUND,
            FILE_NOT_FOUND,
			FILE_CONFIG_INCORRECT,
			LIBRARY_ERROR,
			NETWORK_ERROR,
			NOT_IMPLEMENTED,
			COUNT
	}; /*  MANY MORE TO BE DEFINED IF NECESSARY  */
public:
	//----- CONSTRUCTORS/DESTRUCTORS -----
	//Without parameters (security)
	GenericException():mDescription("Undefined"),mCode(NOT_IMPLEMENTED)
	{}
	GenericException(const std::string &desc, ExceptionCode type):mDescription(desc),mCode(type)
	{}
	~GenericException() throw() //Compatibility with std::exception
	{}
	//----- GET/SET FUNCTIONS -----
	virtual const std::string &GetDescription() const;
	ExceptionCode GetCode() const { return mCode; }
	//----- OTHER FUNCTIONS -----
	// Override base class (std::exception) what method
	const char* what() const throw() //Notice that does not throw exceptions
	{ 
		return GetDescription().c_str(); 
	}
	//Implement an std::string variant
	const std::string& What_String() const throw() //Notice that does not throw exceptions
	{
		return GetDescription();
	}
	//----- PUBLIC VARIABLES ------
protected:
	//----- INTERNAL VARIABLES -----
	std::string mDescription;
	ExceptionCode mCode;
	//----- INTERNAL FUNCTIONS -----
};

#endif