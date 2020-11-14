#pragma once

#include <Windows.h>

template<typename R, typename F, typename ...A>
inline R stdcall(F f, A ...a)
{
	return reinterpret_cast<R(__stdcall*)(A...)>(f)(a...);
}

inline void Exploit(HMODULE lib, HMODULE api, LPCSTR name, LPVOID proc)
{
	if (!lib || !name || !proc)
		return;

	UINT_PTR oldProc = (UINT_PTR)GetProcAddress(api, name);
	if (!oldProc)
		return;

	if (PIMAGE_DOS_HEADER(lib)->e_magic != IMAGE_DOS_SIGNATURE)
		return;

	PIMAGE_NT_HEADERS headers = PIMAGE_NT_HEADERS((LONG)lib + PIMAGE_DOS_HEADER(lib)->e_lfanew);
	if (headers->Signature != IMAGE_NT_SIGNATURE)
		return;

	PIMAGE_IMPORT_DESCRIPTOR desc = (PIMAGE_IMPORT_DESCRIPTOR)((LONG)lib + headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	for (; desc->FirstThunk; desc++)
		if (GetModuleHandle((LPSTR)((LONG)lib + desc->Name)) == api)
			break;

	PIMAGE_THUNK_DATA thunk = (PIMAGE_THUNK_DATA)((LONG)lib + desc->FirstThunk);
	if (!thunk)
		return;

	for (; thunk->u1.Function; thunk++)
	{
		UINT_PTR* address = (UINT_PTR*)&thunk->u1.Function;
		if (*address == oldProc)
		{
			DWORD dwOldProtect = NULL;

			VirtualProtect((LPVOID)address, 4, PAGE_WRITECOPY, &dwOldProtect);
			*address = (UINT_PTR)proc;
			VirtualProtect((LPVOID)address, 4, dwOldProtect, NULL);

			return;
		}
	}
}