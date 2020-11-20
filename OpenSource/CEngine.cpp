#include "CEngine.h"

#include <document.h>
#include <filereadstream.h>
#include <vector>

#include "Utils.h"
#include "Memory.h"

CEngine::CEngine(HMODULE hGame): m_hGame(hGame)
{
	m_DataTable["ModName"] = (DWORD)strcopy("MM Engine");
	m_DataTable["ModIcon"] = (DWORD)strcopy("MMEngine.ico");
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
				HANDLE hMpq;

				SFileOpenArchive((path + "Mpqs\\" + mpq["Name"].GetString() + ".mpq").c_str(), (mpq.HasMember("Priority") && mpq["Priority"].IsInt()) ? mpq["Priority"].GetInt() : m_DataTable["Priority"]++, 0, &hMpq);
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