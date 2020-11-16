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

std::string GetParam(std::string lpCmdLine, std::string Key)
{
	std::string line = lpCmdLine;
	size_t i;

	for (i = 0; i < line.size(); i++)
		if (!_strnicmp(&line[i - Key.size()], std::string("-" + Key).c_str(), Key.size() + 1) && (i == line.size() - 1 || line[i + 1] == ' '))
			break;

	if (i == line.size() - 1)
		return "";

	for (i++; line[i] == ' '; i++)
	{ }

	line = line.substr(i, line.size() - i);

	for (i = 0; i < line.size(); i++)
		if (line[i] == ' ' && line[i + 1] == '-' && line[i + 2] != ' ' && i + 2 < line.size())
			break;

	line = line.substr(0, i);

	for (i = line.size() - 1; i > 0 && line[i] == ' '; i--)
	{ }

	i++;

	line = line.substr(0, i);

	return line;
}

BOOL WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	engine = new CEngine(hGame);

	HMODULE hUser32 = GetModuleHandle("user32.dll");
	Exploit(hGame, hUser32, "CreateWindowExA", CreateWindowExA_Proxy);
	Exploit(hGame, hUser32, "SetWindowTextA", SetWindowTextA_Proxy);
	Exploit(hGame, hUser32, "LoadCursorA", LoadCursorA_Proxy);
	Exploit(hGame, hUser32, "LoadImageA", LoadImageA_Proxy);

	engine->StartGame(GetParam(lpCmdLine, "mod"));
	delete engine;

	return TRUE;
}

//---------------------------------------------------------------------------

HWND WINAPI CreateWindowExA_Proxy(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	if (!_strcmpi(lpWindowName, "warcraft iii"))
		return CreateWindowEx(dwExStyle, lpClassName, (LPCSTR)engine->GetData("ModName"), dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	return CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}

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

HANDLE WINAPI LoadImageA_Proxy(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad)
{
	if (!_strcmpi(name, "war3x.ico"))
		return LoadImage(hInst, "MMEngine.ico", type, cx, cy, fuLoad);
		/*if (!strcmpi(l_lpIconName, "MMEngine.ico"))
			return LoadImage(hInst, l_lpIconName, type, cx, cy, fuLoad);
		else if (FileExists(l_lpIconName))
			return LoadImage(NULL, l_lpIconName, IMAGE_ICON, NULL, NULL, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		else
			return LoadImage(hInst, "MMEngine.ico", type, cx, cy, fuLoad);*/

	return LoadImage(hInst, name, type, cx, cy, fuLoad);
}