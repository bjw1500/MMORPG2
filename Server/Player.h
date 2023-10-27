#pragma once

#include "Creature.h"
#include "Inventory.h"
#include "Account.h"

class Player : public Creature
{
public:
	Player();
	~Player();

	virtual void Update(float deltaTime) override;
	
	//In Game
	void PickItem(shared_ptr<Item> item);
	void EquippedItem(shared_ptr<Item>& item);
	void UseItem(shared_ptr<Item> item);
	void Additem(shared_ptr<Item> item);
	void BuyItem(int32 itemTemplatedId);
	void AcceptQuest(int32 questTemplatedId);
	void RefreshQuest();
	void RefreshInven();
	void CheckQuestTarget(int32 targetTemplatedId);
	virtual bool OnDead(Protocol::ObjectInfo damageCauser) override;
	void SetUsingWeapon(Protocol::ItemInfo* weapon);
	void AddExp(int32 value);
	void Disconnect();


	//Pre Game
	void LoadPlayerDb();
	void SetPlayerDb(shared_ptr<PlayerDb> playerDb) {  _playerDb = playerDb; LoadPlayerDb();}




	Protocol::ItemInfo* GetUsingWeapon() { return _usingWeapon.get(); }
	int32 GetPlayerDbId() { return _playerDb->PlayerDbId; }
	Inventory* GetInven() { return &_inven; }
	shared_ptr<PlayerDb> GetPlayerDb() { return _playerDb; }
	int32 GetGold() { return _inven.GetGold(); }
	int32 GetExp() { return _info.stat().totalexp(); }
	GameSessionRef GetSession() { return _session; }

public:
	GameSessionRef _session;
private:
	Inventory _inven;
	shared_ptr<PlayerDb> _playerDb;
	shared_ptr<Protocol::ItemInfo> _usingWeapon;

};

