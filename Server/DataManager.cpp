#include "pch.h"
#include <fstream>
#include <ostream>
#include "DataManager.h"
#pragma warning(disable: 4996)   

DataManager::DataManager()
{
	ItemTypeMap["WEAPON"] = Protocol::ItemType::WEAPON;
	ItemTypeMap["ARMOR"] = Protocol::ItemType::ARMOR;
	ItemTypeMap["COUNSUMABLE"] = Protocol::ItemType::CONSUMEABLE;


	LoadItemTable();
	LoadMonsterTable();

}

void DataManager::LoadData()
{
}

void DataManager::LoadItemTable()
{
	ifstream ist("Data\\ItemTable.json");
	if (ist.is_open() == false)
	{
		cout << "잘못된 경로." << endl;
		return;
	}

	string str;
	for (char p; ist >> p;)
		str += p;

	Json::Reader reader;
	Json::Value root;
	bool parsingRet = reader.parse(str, root);


	for (int index = 0; index < root.size(); index++)
	{
		Protocol::ItemInfo item;
		item.set_name(root[index]["Name"].asString());
		item.set_templateid(root[index]["Id"].asInt());
		item.set_damage(root[index]["Damage"].asInt());
		item.set_armor(root[index]["Armor"].asInt());
		Protocol::ItemType type = ItemTypeMap[root[index]["ItemType"].asString()];
		item.set_itemtype(type);

		ItemTable[item.templateid()] = item;
	}
}

void DataManager::LoadMonsterTable()
{
	ifstream ist("Data\\MonsterTable.json");
	if (ist.is_open() == false)
	{
		cout << "잘못된 경로." << endl;
		return;
	}

	string str;
	for (char p; ist >> p;)
		str += p;

	Json::Reader reader;
	Json::Value root;
	bool parsingRet = reader.parse(str, root);


	for (int index = 0; index < root.size(); index++)
	{
		MonsterData data;
		data.MonsterName = root[index]["MonsterName"].asString();
		data.Id = root[index]["Id"].asInt();

		Json::Value stat = root[index]["Stat"];
		data.Stat.set_level(stat["Level"].asInt());
		data.Stat.set_maxhp(stat["MaxHp"].asInt());
		data.Stat.set_hp(stat["MaxHp"].asInt());
		data.Stat.set_totalexp(stat["TotalExp"].asInt());
		data.Stat.set_damage(stat["Attack"].asInt());
		data.Stat.set_movespeed(stat["Speed"].asInt());
		data.Stat.set_searchrange(stat["SearchRange"].asInt());
		data.Stat.set_attackrange(stat["AttackRange"].asInt());

		Json::Value reward = root[index]["Reward"];

		data.Reward.ItemDropRate = reward["ItemDropRate"].asInt();
		data.Reward.Id = reward["Id"].asInt();
		data.Reward.Count = reward["Count"].asInt();

		MonsterTable[data.Id] = data;
	}
}

void DataManager::LoadStatTable()
{
	ifstream ist("Data\\Stat.json");
	if (ist.is_open() == false)
	{
		cout << "잘못된 경로." << endl;
		return;
	}

	string str;
	for (char p; ist >> p;)
		str += p;

	Json::Reader reader;
	Json::Value root;
	bool parsingRet = reader.parse(str, root);


	for (int index = 0; index < root.size(); index++)
	{
		Protocol::Stat stat;
		stat.set_level(root[index]["Level"].asInt());
		stat.set_hp(root[index]["Hp"].asInt());
		stat.set_damage(root[index]["Attack"].asInt());

		StatTable[stat.level()] = stat;
	}
}


MonsterData DataManager::GetMonsterData(int32 id)
{
	if (MonsterTable.contains(id) == false)
	{
		cout << "Get Monster Data Error. 없는 데이터입니다." << endl;
	}

	return MonsterTable[id];
}

ItemInfo DataManager::GetItemData(int32 id)
{
	if (ItemTable.contains(id) == false)
	{
		cout << "Get Item Data Error. 없는 데이터입니다." << endl;
	}

	return ItemTable[id];
}


//참고용 샘플 코드
void DataManager::ParseTest()
{
	ifstream ist("test.json");
	string str;
	for (char p; ist >> p;)
		str += p;

	Json::Reader reader;
	Json::Value root;
	bool parsingRet = reader.parse(str, root);


	cout << root["name"] << endl;
	cout << root["age"] << endl;
	cout << root["address"] << endl;
	cout << root["gfriend"] << endl;

	Json::Value family = root["family"];
	cout << family[0].asString() << endl;
	cout << family[1].asString() << endl;
	cout << family[2].asString() << endl;
}