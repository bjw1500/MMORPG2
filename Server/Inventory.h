#pragma once
#include "Item.h"

#define MaxSlot 24

class Inventory
{
public:
	Inventory();
	~Inventory();

	void AddItem(shared_ptr<Item> item);
	void RemoveItem(shared_ptr<Item> item);

	void AddGold(int32 gold);
	void SubGold(int32 gold);

	shared_ptr<Item> GetItem(int32 itemDbId);
	int32 GetEmptySlot();
	map<int32, shared_ptr<Item>> GetItemList() { return _items; }
	int32 GetGold() { return _gold; }

private:

	int32 _gold = 0;
	map<int32, shared_ptr<Item>> _items;

};

