#pragma once

extern "C"
{
	inline unsigned int strsize(const char* str)
	{
		unsigned int retval = 0;

		for (retval; str[retval] != 0; retval++)
		{
		}

		return retval;
	}

	inline void strfill(char* str, int val, unsigned int size)
	{
		for (unsigned int i = 0; i < size; i++)
			str[i] = val;
	}

	inline char* strcopy(const char* str)
	{
		unsigned int size = strsize(str);
		char* retval = new char[size];
		strfill(retval, 0, size + 1);

		for (unsigned int i = 0; i < size; i++)
			retval[i] = str[i];

		return retval;
	}
}