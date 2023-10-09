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

	SkillTypeMap["DASH"] = Protocol::Skill_ID::DASH;
	SkillTypeMap["SKILL_1"] = Protocol::Skill_ID::SKILL_1;
	SkillTypeMap["SKILL_2"] = Protocol::Skill_ID::SKILL_2;
	SkillTypeMap["SKILL_3"] = Protocol::Skill_ID::SKILL_3;

	LoadItemTable();
	LoadMonsterTable();
	LoadSkillTable();
	LoadCharacterTable();
	LoadQuestTable();

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
		item.set_price(root[index]["Price"].asInt());
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

		Json::Value skill = stat["Skills"];
		for (int skillIndex = 0; skillIndex < skill.size(); skillIndex++)
		{
			data.Stat.add_skills(skill[skillIndex].asString());
		}


		Json::Value reward = root[index]["RewardDatas"];

		for (int rewardIndex = 0; rewardIndex < reward.size(); rewardIndex++)
		{
			FRewardData rewardData;

			rewardData.ItemDropRate = reward[rewardIndex]["ItemDropRate"].asInt();
			rewardData.Id = reward[rewardIndex]["Id"].asInt();
			rewardData.Count = reward[rewardIndex]["Count"].asInt();
			data.RewardDatas.push_back(rewardData);
		}

		MonsterTable[data.MonsterName] = data;
	}
}

void DataManager::LoadSkillTable()
{
	ifstream ist("Data\\SkillTable.json");
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
		FSkillData skill;
		skill.Name = root[index]["Name"].asString();
		skill.Description = root[index]["Description"].asString();

		string type = root[index]["SkillType"].asString();
		skill.SkillType_ID = SkillTypeMap[type];

		skill.CoolTime = root[index]["CoolTime"].asInt();
		skill.Damage = root[index]["Damage"].asInt();


		SkillTable[skill.Name] = skill;
	}
}

void DataManager::LoadCharacterTable()
{
	ifstream ist("Data\\CharacterTable.json");
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
		FCharacterData data;
		data.CharacterName = root[index]["CharacterName"].asString();
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

		Json::Value skill = stat["Skills"];
		for (int skillIndex = 0; skillIndex < skill.size(); skillIndex++)
		{
			data.Stat.add_skills(skill[skillIndex].asString());
		}

		CharacterTable[data.Id] = data;
	}
}

void DataManager::LoadQuestTable()
{
	ifstream ist("Data\\QuestTable.json");
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
		FQuestData data;
		data.Id = root[index]["Id"].asInt();
		data.TargetTemplatedId = root[index]["TargetTemplatedId"].asInt();
		data.TargetNumbers = root[index]["TargetNumbers"].asInt();
		QuestTable[data.Id] = data;
	}

	//949로 저장해주기
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


MonsterData DataManager::GetMonsterData(string name)
{
	if (MonsterTable.contains(name) == false)
	{
		cout << "Get Monster Data Error. 없는 데이터입니다." << endl;
	}

	return MonsterTable[name];
}

ItemInfo DataManager::GetItemData(int32 id)
{
	if (ItemTable.contains(id) == false)
	{
		cout << "Get Item Data Error. 없는 데이터입니다." << endl;
	}

	return ItemTable[id];
}

FCharacterData DataManager::GetCharacterData(int32 id)
{
	if (CharacterTable.contains(id) == false)
	{
		cout << "Get Item Data Error. 없는 데이터입니다." << endl;
	}

	return CharacterTable[id];
}

FSkillData DataManager::GetSkillData(string name)
{
	if (SkillTable.contains(name) == false)
	{
		cout << "Get Skill Data Error. 없는 데이터입니다." << endl;
	}

	return SkillTable[name];
}

FQuestData DataManager::GetQuestData(int32 id)
{
	if (QuestTable.contains(id) == false)
	{
		cout << "Get Quest Data Error. 없는 데이터입니다." << endl;
	}

	return QuestTable[id];
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