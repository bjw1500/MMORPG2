// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemList.h"
#include "UI/ShopItemSlot.h"
#include "Components/ListView.h"
#include "Blueprint/IUserObjectListEntry.h"


void UShopItemList::RefreshUI()
{
	//판매할 수 있는 아이템 리스트를 확인하여 목록에 넣어준다.
	ShopItemList->ClearListItems();

	for (FItemData itemData : ItemList)
	{
		UShopItemSlot* newElement = Cast<UShopItemSlot>(CreateWidget(GetWorld(), ListElement));
		newElement->SetElement(itemData);
		newElement->Master = this;
		ShopItemList->AddItem(newElement);
	}
}
