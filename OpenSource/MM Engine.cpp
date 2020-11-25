#include <Windows.h>
#include <vector>
#include <string>

#include "Memory.h"
#include "CEngine.h"
#include "Utils.h"

extern "C" HMODULE hGame = LoadLibrary("game.dll");
CEngine* engine = nullptr;

//---------------------------------------------------------------------------

HWND WINAPI CreateWindowExA_Proxy(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
BOOL WINAPI SetWindowTextA_Proxy(HWND hWnd, LPCSTR lpString);
HCURSOR WINAPI LoadCursorA_Proxy(HINSTANCE hInstance, LPCSTR lpCursorName);
HANDLE WINAPI LoadImageA_Proxy(HINSTANCE hInst, LPCSTR name, UINT type, int cx, int cy, UINT fuLoad);

int __cdecl SStrVPrintf_Proxy(char* dest, size_t size, const char* format, void* a...);

//---------------------------------------------------------------------------

extern "C" {
	std::string& GetRace(UINT index);

	std::string& GetRaceName(UINT index);

	size_t GetRacesCount();

	size_t GetRacesNamesCount();

	void RaceUI();

	void RaceSounds();

	void RaceLoadingScreen();

	void RaceName();

	void RaceScoreScreen();

	void RaceOrder();

	void RaceSlot();

	void RaceStartUnits();

	void RaceBlocked();

	void RaceIncreaser();

	void RaceInit();
}

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

	DWORD dwGame = (DWORD)hGame;
	jmp(dwGame + 0x3a2840, RaceUI);
	jmp(dwGame + 0x31f5d0, RaceSounds);
	jmp(dwGame + 0x5a3d84, RaceLoadingScreen);
	jmp(dwGame + 0x58aa02, RaceName);
	jmp(dwGame + 0x39f710, RaceScoreScreen);
	jmp(dwGame + 0x559580, RaceOrder);
	jmp(dwGame + 0x5bed8e, dwGame + 0x5bedab);
	jmp(dwGame + 0x559260, RaceSlot);
	jmp(dwGame + 0x3a31a0, RaceStartUnits);
	jmp(dwGame + 0x599bcc, RaceBlocked);
	fill(dwGame + 0x5c0a1b, 0x90, 6);
	fill(dwGame + 0x5c0a25, 0x90, 5);
	jmp(dwGame + 0x3c11d0, RaceIncreaser);
	jmp(dwGame + 0x3c11a0, RaceInit);
	patch(dwGame + 0x5bf4e3, 0, 1);

	Exploit(hGame, GetModuleHandle("storm.dll"), (LPCSTR)578, SStrVPrintf_Proxy);

	engine->StartGame(GetParam(lpCmdLine, "mod"));
	delete engine;

	return TRUE;
}
//---------------------------------------------------------------------------

extern "C" {
	std::string& GetRace(UINT index)
	{
		return (*(std::vector<std::string>*)engine->GetData("Skins"))[index];
	}

	std::string& GetRaceName(UINT index)
	{
		return (*(std::vector<std::string>*)engine->GetData("Keys"))[index];
	}

	size_t GetRacesCount()
	{
		return (*(std::vector<std::string>*)engine->GetData("Skins")).size();
	}

	size_t GetRacesNamesCount()
	{
		return (*(std::vector<std::string>*)engine->GetData("Keys")).size();
	}
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
	{
		LPCSTR icon = (LPCSTR)engine->GetData("ModIcon");

		if (!_strcmpi(icon, "MMEngine.ico"))
			return LoadImage(hInst, icon, type, cx, cy, fuLoad);
		else if (FileExists(icon))
			return LoadImage(NULL, icon, IMAGE_ICON, NULL, NULL, LR_LOADFROMFILE | LR_DEFAULTSIZE);
		else
			return LoadImage(hInst, "MMEngine.ico", type, cx, cy, fuLoad);
	}

	return LoadImage(hInst, name, type, cx, cy, fuLoad);
}

int SStrVPrintf_Analog(char* dest, size_t size, void* a ...)
{
	return SStrVPrintf(dest, size, (LPCSTR)engine->GetData("ModVersion"), a);
}

int __cdecl SStrVPrintf_Proxy(char* dest, size_t size, const char* format, void* a ...)
{
	if (!strcmp(format, "%d.%d.%d.%d"))
		return SStrVPrintf_Analog(dest, size, a);

	return SStrVPrintf(dest, size, format, a);
}