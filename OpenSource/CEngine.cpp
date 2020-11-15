#include "CEngine.h"

#include <document.h>
#include <filereadstream.h>
#include <vector>

#include "StringUtils.h"
#include "Memory.h"

CEngine::CEngine(HMODULE hGame): m_hGame(hGame)
{
	m_DataTable["ModName"] = (DWORD)strcopy("MM Engine");
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
	FILE* json = fopen((".\\Mods\\" + ModName + "\\Manifest.json").c_str(), "rb");

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
}