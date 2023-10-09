#pragma once
#include "DataContents.h"

class Account
{
public:
	Account();
	Account(string accountName, int32 dbId);
	~Account();

	int32 GetAccountDbId() { return _accountDbId; }
	string GetAccountName() { return _accountName; }

public:
	vector<shared_ptr<class PlayerDb>> _players;

private:
	int32 _accountDbId;
	string _accountName;
};


class PlayerDb
{
public:
	PlayerDb();
	~PlayerDb();

	Protocol::ObjectInfo GetPlayerInfo();
	Protocol::ItemInfo GetCurrentUseWeapon();
	void AddQuest(int32 templatedId);

public:
	int32 PlayerDbId;
	int32 PlayerSlot;
	string PlayerName;

	//Account
	int32 AccountDbId;
	shared_ptr<Account> AccountDb;

	//Stat
	int32 Level;
	int32 Hp;
	int32 MaxHp;
	int32 Attack;
	int32 TotalExp;

	Protocol::Position Pos;

	//Contents
	vector<shared_ptr<class ItemDb>> Items;
	int32 Gold;
	//영웅 모델링 ID
	int32 TemplatedId = 1;

	//Quest
	map<int32, FQuestData> CurrentQuestList;
};

class ItemDb
{
public:
	ItemDb();
	~ItemDb();

	Protocol::ItemInfo GetItemInfo();

public:
	int32 ItemDbId;
	int32 TemplatedId;
	int32 Count;
	int32 Slot;
	bool IsEquipped;

	//Player
	int32 OwnerDbId;
	shared_ptr<PlayerDb> Owner;
};
