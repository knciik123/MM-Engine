#include "CEngine.h"

#include "Memory.h"

CEngine::CEngine(HMODULE hGame): m_hGame(hGame)
{
	m_DataTable["WindowName"] = (LPVOID)"MM Enginee";
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
	stdcall<BOOL>(procGameMain, m_hGame);
}

LPVOID CEngine::GetData(std::string Key)
{
	std::map<std::string, LPVOID>::iterator it = m_DataTable.find(Key);

	return it == m_DataTable.end() ? 0 : it->second;
}

void CEngine::LoadManifest(std::string)
{

}