#include "pch.h"
#include "Inventory.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::AddItem(shared_ptr<Item> item)
{
	_items[item->GetItemDbId()] = item;

}

void Inventory::RemoveItem(shared_ptr<Item> item)
{
	if (_items.contains(item->GetItemDbId()) == false)
		return;

	_items.erase(item->GetItemDbId());
}

void Inventory::AddGold(int32 gold)
{
	_gold += gold;
}

void Inventory::SubGold(int32 gold)
{
	_gold -= gold;
}

shared_ptr<Item> Inventory::GetItem(int32 itemDbId)
{
	if (_items.contains(itemDbId) == false)
	{
		cout << "Inventory::GetItem Error" << itemDbId << endl;
		return nullptr;
	}

	return _items[itemDbId];
}

int32 Inventory::GetEmptySlot()
{
	if (_items.size() == 0)
	{
		return 0;
	}


	for (int i = 0; i < MaxSlot ; i++)
	{
		bool usingSlot = false;
		for (auto item : _items)
		{
			if (i == item.second->GetItemInfo()->slot())
			{
				usingSlot = true;
				break;
			}
		}

		if (usingSlot == false)
			return i;
	}

	return -1;
}
