#include "CEngine.h"

#include <document.h>
#include <filereadstream.h>

#include "Memory.h"

CEngine::CEngine(HMODULE hGame): m_hGame(hGame)
{
	m_DataTable["WindowName"] = (DWORD)"MM Enginee";
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
		LoadManifest(ModName);

	stdcall<BOOL>(procGameMain, m_hGame);
}

void CEngine::LoadConfigs()
{
	FILE* json;

	json = fopen("MM Engine.json", "rb");

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

	fclose(json);
	delete buffer;

	if (!doc.IsObject())
		return;

	for (auto it = doc.MemberBegin(); it < doc.MemberEnd(); it++)
		if (!it->value.IsObject() && !it->value.IsArray())
			m_DataTable[it->name.GetString()] = it->value.IsString() ? (DWORD)it->value.GetString() : it->value.IsBool() ? it->value.GetBool() : it->value.IsInt() ? it->value.GetInt() : (DWORD)it->value.GetDouble();
}

DWORD CEngine::GetData(std::string Key)
{
	std::map<std::string, DWORD>::iterator it = m_DataTable.find(Key);

	return it == m_DataTable.end() ? 0 : it->second;
}

void CEngine::LoadManifest(std::string ModName)
{
	FILE* Json;

	Json = fopen(ModName.c_str(), "rb");

	if (!Json)
		return;

	
	// Test
}