#ifndef INCLUDES_H //Define Guard
#define INCLUDES_H
#define WIN32_LEAN_AND_MEAN //Excludes Headers We Wont Use (Increase Compile Time)

#include <windows.h>	//Windows-functions (OpenProcess, RPM, WPM, etc)
#include <tlhelp32.h>	//Functions that gather process-information

#include <map>			//Data-container that saves parsed modules 
#include <string>		//String/WString implementation
#include <cctype>		//tolower-function that converts a char to lowercase
#include <algorithm>	//transform-function that is used to apply the tolower-function to a wstring
#include <cassert>		//Used for debugging
#include <type_traits>	//Used to restrict usage of template functions Read and Write to specific types
#include <iostream>		//cout
#include <memory>		//unique_ptr
#include <vector>

#endif