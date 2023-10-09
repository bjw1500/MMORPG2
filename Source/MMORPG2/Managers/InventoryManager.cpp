// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/InventoryManager.h"
#include "Object/MyItem.h"

void UInventoryManager::AddItem(Protocol::ItemInfo* Info)
{
	Items.Add(Info->slot(), *Info);
}

Protocol::ItemInfo* UInventoryManager::GetItem(int ItemDbId)
{
	bool ret = Items.Contains(ItemDbId);
	if (ret == false)
		return nullptr;

	return Items.Find(ItemDbId);

}
