/*
	Filename: GeneralException.cpp
	Copyright: Miguel Angel Quinones (mikeskywalker007@gmail.com)
	Description: Class to encapsulate generic exceptions in program - derived from std::exception, so it
				 contains handling to generic library and memory allocation failures
	Comments: 
	Attribution: 
	License: You are free to use as you want... but it can destroy your computer, so dont blame me about it ;)
	         Nevertheless it would be nice if you tell me you are using something I made, just for curiosity 
*/


#include "GenericException.h"

//Return text for description
const std::string &GenericException::GetDescription() const
{
	return(mDescription);
}
