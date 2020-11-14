#pragma once

#include <Windows.h>
#include <Storm.h>
#include <string>
#include <map>

class CEngine
{
private:
	HMODULE m_hGame;
	std::map<std::string, LPVOID> m_DataTable;

	void LoadManifest(std::string ModName);
public:
	CEngine(HMODULE hGame);
	~CEngine();

	void StartGame(std::string ModName);

	LPVOID GetData(std::string Key);
};