#pragma once
#include "DataContents.h"


class DataManager
{
public:
	DataManager();


	map<int32, Protocol::ItemInfo> ItemTable;
	map<int32, Protocol::Stat> StatTable;
	map<int32, MonsterData> MonsterTable;

	void LoadData();
	void LoadStatTable();
	void LoadItemTable();
	void LoadMonsterTable();



public:

	MonsterData GetMonsterData(int32 id);
	ItemInfo GetItemData(int32 id);

private:
	map<string, Protocol::ItemType> ItemTypeMap;


public:
	//sample 코드
	void ParseTest();

};

