// Dev
#include <Windows.h>
#include <string>

#include "Memory.h"
#include "CEngine.h"

HMODULE hGame = LoadLibrary("game.dll");
CEngine* engine = nullptr;

//---------------------------------------------------------------------------

HWND WINAPI CreateWindowExA_Proxy(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI SetWindowTextA_Proxy(HWND hWnd, LPCSTR lpString);
HCURSOR WINAPI LoadCursorA_Proxy(HINSTANCE hInstance, LPCSTR lpCursorName);
HANDLE WINAPI LoadImageA_Proxy(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);

//---------------------------------------------------------------------------

BOOL WINAPI SetWindowTextA_Proxy(HWND hWnd, LPCSTR lpString);

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	engine = new CEngine(hGame);

	Exploit(hGame, GetModuleHandle("user32.dll"), "SetWindowTextA", SetWindowTextA_Proxy);

	engine->StartGame("Naga Race");
	delete engine;

	return TRUE;
}

//---------------------------------------------------------------------------

/*HWND WINAPI CreateWindowExA_Proxy(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (!strcmpi(lpWindowName, "warcraft iii"))
		return CreateWindowEx(dwExStyle, lpClassName, l_lpModName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	return CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}*/

BOOL WINAPI SetWindowTextA_Proxy(HWND hWnd, LPCSTR lpString)
{
	if (!_strcmpi(lpString, "warcraft iii"))
		return SetWindowText(hWnd, (LPCSTR)engine->GetData("ModName"));

	return SetWindowText(hWnd, lpString);
}

HCURSOR WINAPI LoadCursorA_Proxy(HINSTANCE hInstance, LPCSTR lpCursorName)
{
	if (!_strcmpi(lpCursorName, "blizzardcursor.cur"))
		return LoadCursor(hInstance, "MMEngineCursor.cur");

	return LoadCursor(hInstance, lpCursorName);
}

/*HANDLE WINAPI LoadImageA_Proxy(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
	if (!strcmpi(name, "war3x.ico"))
		if (!strcmpi(l_lpIconName, "MMEngine.ico"))
			return LoadImage(hInst, l_lpIconName, type, cx, cy, fuLoad);
		else if (FileExists(l_lpIconName))
			return LoadImage(NULL, l_lpIconName, IMAGE_ICON, NULL, NULL, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		else
			return LoadImage(hInst, "MMEngine.ico", type, cx, cy, fuLoad);

	return LoadImage(hInst, name, type, cx, cy, fuLoad);
}*/