#include "pch.h"
#include "Item.h"
#include "DataManager.h"

shared_ptr<Item> Item::MakeItem(Protocol::ItemInfo* itemInfo)
{
	shared_ptr<Item> item = make_shared<Item>();
	item->SetItemInfo(*itemInfo);

	return item;
}

shared_ptr<Item> Item::MakeItem(shared_ptr<ItemDb> itemDb)
{
	Protocol::ItemInfo info = GDataManager->GetItemData(itemDb->TemplatedId);
	
	info.set_slot(itemDb->Slot);
	info.set_isequipped(itemDb->IsEquipped);
	info.set_count(itemDb->Count);
	info.set_databaseid(itemDb->ItemDbId);

	shared_ptr<Item> item = make_shared<Item>();
	item->SetItemInfo(info);

	return item;
}
