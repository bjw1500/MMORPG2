#pragma once
#include "GameObject.h"
#include "Account.h"
class Item : public GameObject
{
public:
	static  shared_ptr<Item> MakeItem(Protocol::ItemInfo* itemInfo);
	static shared_ptr<Item> MakeItem(shared_ptr<ItemDb> itemDb);

	void SetItemInfo(Protocol::ItemInfo itemInfo) { Info.CopyFrom(itemInfo); }

	ItemInfo* GetItemInfo() { return &Info; }
	int32 GetItemDbId() { return Info.databaseid(); }
	bool GetIsEquipped() { return Info.isequipped(); }
	ItemType GetItemType() { return Info.itemtype(); }

private:
	Protocol::ItemInfo Info;
};


class Weapon : public Item
{


};


class Armor : public Item
{


};


class Consumable : public Item
{



};

