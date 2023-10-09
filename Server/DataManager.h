#pragma once
#include "DataContents.h"


class DataManager
{
public:
	DataManager();




	void LoadData();
	void LoadStatTable();
	void LoadItemTable();
	void LoadMonsterTable();
	void LoadSkillTable();
	void LoadCharacterTable();
	void LoadQuestTable();


public:

	MonsterData GetMonsterData(string name);
	ItemInfo GetItemData(int32 id);
	FCharacterData GetCharacterData(int32 id);
	FSkillData GetSkillData(string name);
	FQuestData GetQuestData(int32 id);

private:
	map<string, Protocol::ItemType> ItemTypeMap;
	map<string, Protocol::Skill_ID> SkillTypeMap;

	map<int32, Protocol::ItemInfo> ItemTable;
	map<int32, Protocol::Stat> StatTable;
	map<string, MonsterData> MonsterTable;
	map<string, FSkillData> SkillTable;
	map<int32, FCharacterData> CharacterTable;
	map<int32, FQuestData> QuestTable;

public:
	//sample 코드
	void ParseTest();

};

