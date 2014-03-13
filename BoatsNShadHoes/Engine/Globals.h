#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <DirectXMath.h>
#include "dxerr.h"

// Convenience macro for releasing a COM object
#define ReleaseMacro(x) { if(x){ x->Release(); x = 0; } }

// Macro for popping up a text box based
// on a failed HRESULT and then quitting (only in debug builds)
#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)												\
	{															\
		HRESULT hr = (x);										\
		if(FAILED(hr))											\
		{														\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);	\
			PostQuitMessage(0);									\
		}														\
	}														
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
#endif

static std::string* Split(std::string s, char delimeter)
{
	int delimCount = std::count(s.begin(), s.end(), delimeter);
	std::string* splitString = new std::string[delimCount + 1];
	int index = 0;

	for (std::size_t i = 0; i < s.length(); i++)
	{
		if (s[i] != delimeter)
			splitString[index] += s[i];
		else
			index++;
	}

	return splitString;
};

#endif