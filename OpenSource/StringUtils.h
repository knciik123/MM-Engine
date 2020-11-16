#pragma once

#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else

typedef const char* LPCSTR;
typedef char* LPSTR;
typedef unsigned int size_t;

#endif

#ifndef __cplusplus
extern "C"
{
#endif

	inline LPSTR strcopy(LPCSTR str)
	{
		size_t size = strlen(str);
		char* retval = new char[size];
		memset(retval, 0, size + 1);

		for (size_t i = 0; i < size; i++)
			retval[i] = str[i];

		return retval;
	}

#ifndef __cplusplus
}
#endif