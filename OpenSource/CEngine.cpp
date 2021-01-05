#include "CEngine.h"

#include <document.h>
#include <filereadstream.h>
#include <vector>

#include "Utils.h"
#include "Memory.h"

void ShowLogo(LPSTR l_lpMod, LPSTR l_lpFileName);
LRESULT CALLBACK LogoWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LPSTR l_lpInfo;
HINSTANCE l_hInstance = GetModuleHandle(NULL);
HBITMAP l_hBmp;

//---------------------------------------------------------------------------------------------------------------

CEngine::CEngine(HMODULE hGame) : m_hGame(hGame)
{
	m_DataTable["Mod"] = 0;
	m_DataTable["ModName"] = (DWORD)strcopy("MM Engine");
	m_DataTable["ModIcon"] = (DWORD)strcopy("MMEngine.ico");
	m_DataTable["Logo"] = NULL;
	m_DataTable["Info"] = (DWORD)strcopy("© (2020) The Mod Makers");
	m_DataTable["Enable"] = true;
	m_DataTable["ModVersion"] = (DWORD)strcopy("MM Engine - Version 2.0.0 (Dev Build)");
	m_DataTable["Priority"] = 9;
	m_DataTable["Skins"] = (DWORD)(new std::vector<std::string> { "Human", "Orc", "Undead", "NightElf" });
	m_DataTable["Keys"] = (DWORD)(new std::vector<std::string> { "HUMAN", "ORC", "UNDEAD", "NIGHT_ELF" });
}

CEngine::~CEngine()
{
	FreeLibrary(m_hGame);
}

void CEngine::StartGame(std::string ModName)
{
	if (!m_hGame)
		return;
	
	FARPROC procGameMain = GetProcAddress(m_hGame, "GameMain");

	if (!procGameMain)
		return;

	SFileOpenArchive("", NULL, NULL, NULL);

	LoadConfigs();

	if (!ModName.empty())
	{
		m_DataTable["Mod"] = (DWORD)strcopy(ModName.c_str());

		LoadManifest(ModName);
	}

	l_lpInfo = (LPSTR)GetData("Info");

	if (GetData("Enable"))
		ShowLogo((LPSTR)GetData("Mod"), (LPSTR)GetData("Logo"));

	stdcall<BOOL>(procGameMain, m_hGame);
}

void CEngine::LoadConfigs()
{
	FILE* json = fopen("MM Engine.json", "rb");

	if (!json)
		return;

	fseek(json, 0, SEEK_END);
	LONG lenght = ftell(json);
	fseek(json, 0, SEEK_SET);

	LPSTR buffer = new char[lenght];
	memset(buffer, 0, lenght);

	rapidjson::Document doc;
	rapidjson::FileReadStream istream(json, buffer, lenght);
	doc.ParseStream(istream);

	delete[] buffer;
	fclose(json);

	if (!doc.IsObject())
		return;

	std::map<std::string, DWORD>::iterator it_data;

	for (auto it = doc.MemberBegin(); it < doc.MemberEnd(); it++)
	{
		it_data = m_DataTable.find(it->name.GetString());

		if (!it->value.IsObject() && !it->value.IsArray() && it_data == m_DataTable.end())
			m_DataTable[it->name.GetString()] = it->value.IsString() ? (DWORD)strcopy(it->value.GetString()) : it->value.IsBool() ? it->value.GetBool() : it->value.IsInt() ? it->value.GetInt() : (DWORD)it->value.GetDouble();
	}
}

DWORD CEngine::GetData(std::string Key)
{
	std::map<std::string, DWORD>::iterator it = m_DataTable.find(Key);

	return it == m_DataTable.end() ? 0 : m_DataTable[Key];
}

void CEngine::LoadManifest(std::string ModName)
{
	std::string path = ".\\Mods\\" + ModName + "\\";
	FILE* json = fopen((path + "Manifest.json").c_str(), "rb");

	if (!json)
		return;
	
	fseek(json, 0, SEEK_END);
	LONG lenght = ftell(json);
	fseek(json, 0, SEEK_SET);

	LPSTR buffer = new char[lenght];
	memset(buffer, 0, lenght);

	rapidjson::Document doc;
	rapidjson::FileReadStream istream(json, buffer, lenght);
	doc.ParseStream(istream);

	delete[] buffer;
	fclose(json);

	if (!doc.IsObject())
		return;

	if (doc.HasMember("ModName") && doc["ModName"].IsString())
	{
		delete[] (LPSTR)m_DataTable["ModName"];
		m_DataTable["ModName"] = (DWORD)strcopy(doc["ModName"].GetString());
	}
	
	if (doc.HasMember("ModIcon") && doc["ModIcon"].IsString())
	{
		delete[] (LPSTR)m_DataTable["ModIcon"];
		m_DataTable["ModIcon"] = (DWORD)strcopy((path + std::string(doc["ModIcon"].GetString()) + ".ico").c_str());
	}

	if (doc.HasMember("Preview") && doc["Preview"].IsObject())
	{
		if (doc["Preview"].HasMember("Logo") && doc["Preview"]["Logo"].IsString())
			m_DataTable["Logo"] = (DWORD)strcopy(doc["Preview"]["Logo"].GetString());

		if (doc["Preview"].HasMember("Info") && doc["Preview"]["Info"].IsString())
		{
			delete[](LPSTR)m_DataTable["Info"];
			m_DataTable["Info"] = (DWORD)strcopy(doc["Preview"]["Info"].GetString());
		}

		if (doc["Preview"].HasMember("Enable") && doc["Preview"]["Enable"].IsBool())
			m_DataTable["Enable"] = doc["Preview"]["Enable"].GetBool();
	}

	if (doc.HasMember("ModVersion") && doc["ModVersion"].IsString())
	{
		delete[](LPSTR)m_DataTable["ModVersion"];
		m_DataTable["ModVersion"] = (DWORD)strcopy(doc["ModVersion"].GetString());
	}

	if (doc.HasMember("Mpqs") && doc["Mpqs"].IsArray())
		for (size_t it = 0; it < doc["Mpqs"].MemberCount(); it++)
		{
			rapidjson::Value mpq = doc["Mpqs"][it].GetObjectA();

			if (mpq.HasMember("Name") && mpq["Name"].IsString())
			{
				std::string filename = mpq["Name"].GetString();
				std::string filepath = path + "Mpqs\\" + filename + ".mpq";

				if (FileExists(filepath.c_str()))
				{
					HANDLE hMpq;
					
					SFileOpenArchive(filepath.c_str(), (mpq.HasMember("Priority") && mpq["Priority"].IsInt()) ? mpq["Priority"].GetInt() : m_DataTable["Priority"]++, 0, &hMpq);
				}
				else if (m_DataTable["ShowMpqMessage"])
					MessageBox(NULL, ("В моде " + std::string((LPCSTR)m_DataTable["Mod"]) + " отсутствует " + filename + ".mpq").c_str(), "Внимание", MB_ICONWARNING);
			}
		}

	{
		std::vector<std::string>& races = (*(std::vector<std::string>*)m_DataTable["Skins"]);
		std::vector<std::string>& raceskeys = (*(std::vector<std::string>*)m_DataTable["Keys"]);

		for (auto r : races)
			r.~basic_string();

		for (auto r : raceskeys)
			r.~basic_string();

		std::vector<std::string>().swap(races);
		std::vector<std::string>().swap(raceskeys);

		if (doc.HasMember("Races") && doc["Races"].IsArray())
			for (size_t it = 0; it < doc["Races"].MemberCount(); it++)
			{
				rapidjson::Value race = doc["Races"][it].GetObjectA();

				if (race.HasMember("Skin") && race["Skin"].IsString())
				{

					races.push_back(race["Skin"].GetString());
					raceskeys.push_back(race.HasMember("Key") && race["Key"].IsString() ? race["Key"].GetString() : "UNKNOWN");
				}
			}
	}
}

//---------------------------------------------------------------------------------------------------------------

void ShowLogo(LPSTR lpMod, LPSTR lpFileName)
{
	std::string* Logo = new std::string(".\\mods\\" + std::string(lpMod ? lpMod : "") + "\\" + (lpFileName ? lpFileName : "") + ".bmp");

	if (!lpFileName || !FileExists(Logo->c_str()))
		l_hBmp = (HBITMAP)LoadImageA(l_hInstance, "MMEngineBitmap.bmp", IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	else
		l_hBmp = (HBITMAP)LoadImageA(0, Logo->c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	delete Logo;

	if (!l_hBmp)
		return;

	for (size_t i = 0; i < strlen(l_lpInfo); i++)
		if (!strncmp(&l_lpInfo[i], "\\n", 2))
		{
			l_lpInfo[i] = ' ';
			l_lpInfo[i + 1] = '\n';
		}

	WNDCLASSEX LogoClass;
	HWND hLogo;
	MSG Msg;

	LogoClass.cbSize = sizeof(LogoClass);
	LogoClass.lpszClassName = "LogoClass";
	LogoClass.lpfnWndProc = LogoWndProc;
	LogoClass.style = CS_HREDRAW | CS_VREDRAW;
	LogoClass.hInstance = l_hInstance;
	LogoClass.hIcon = LoadIconA(LogoClass.hInstance, "MMEngine.ico");
	LogoClass.hIconSm = LoadIconA(LogoClass.hInstance, "MMEngine.ico");
	LogoClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
	LogoClass.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	LogoClass.lpszMenuName = NULL;
	LogoClass.cbClsExtra = NULL;
	LogoClass.cbWndExtra = NULL;

	RegisterClassExA(&LogoClass);

	RECT Window;
	GetClientRect(GetDesktopWindow(), &Window);
	int LogoWidth = 500;
	int LogoHeight = 400;
	int LogoX = (Window.right - LogoWidth) / 2;
	int LogoY = (Window.bottom - LogoHeight) / 2;
	hLogo = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, "LogoClass", "MM Engine - Mod Logo", WS_POPUP, LogoX, LogoY, LogoWidth, LogoHeight, NULL, NULL, l_hInstance, NULL);

	ShowWindow(hLogo, SW_SHOWNORMAL);
	UpdateWindow(hLogo);

	HANDLE hTimer = CreateEvent(NULL, TRUE, FALSE, NULL);
	while (WaitForSingleObject(hTimer, 1000) != WAIT_TIMEOUT && GetMessage(&Msg, hLogo, 0, 0))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	CloseHandle(hTimer);
	DestroyWindow(hLogo);
}

LRESULT CALLBACK LogoWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HDC hMemDC = CreateCompatibleDC(hdc);

		BITMAP bmp;
		GetObject(l_hBmp, sizeof(BITMAP), &bmp);

		SelectObject(hMemDC, l_hBmp);
		BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hMemDC, 0, 0, SRCCOPY);

		SetTextColor(hdc, 0xC8F0);
		SetBkMode(hdc, TRANSPARENT);

		RECT rect;
		GetClientRect(hWnd, &rect);

		size_t lines = 1;
		for (size_t i = 0; i < strlen(l_lpInfo); i++)
			if (l_lpInfo[i] == '\n')
				lines++;

		rect.top += 400 - 25 * lines + 9 * (lines - 1);
		DrawTextA(hdc, l_lpInfo, -1, &rect, DT_CENTER | DT_TOP | DT_WORDBREAK);

		DeleteDC(hMemDC);
		EndPaint(hWnd, &ps);

		break;
	}
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}