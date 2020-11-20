#pragma once

#include <windows.h>
#include <string.h>

inline LPSTR strcopy(LPCSTR str)
{
	size_t size = strlen(str);
	char* retval = new char[size];
	memset(retval, 0, size + 1);

	for (size_t i = 0; i < size; i++)
		retval[i] = str[i];

	return retval;
}

inline bool FileExists(LPCSTR name)
{
	FILE* file = fopen(name, "rb");

	if (!file)
		return false;

	fclose(file);

	return true;
}