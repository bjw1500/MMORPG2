// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InventoryUI.h"
#include "Object/MyItem.h"
#include "Managers/InventoryManager.h"
#include "UI/ItemSlot.h"
#include "Network/GameManager.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"

bool UInventoryUI::Initialize()
{
	Super::Initialize();

	//슬롯 갯수는 총 24개. 
	Items.Init(nullptr, 24);
	TArray<UWidget*> widgets;
	WidgetTree->GetAllWidgets(widgets);

	for (UWidget* widget : widgets)
	{
		UItemSlot* slot = Cast<UItemSlot>(widget);
		if (IsValid(slot) == false)
			continue;

		TArray<FString> splits;
		FString name = slot->GetFName().ToString();
		int32 indexCount = name.ParseIntoArray(splits, TEXT("_"));
		int32 slotNumber = FCString::Atoi(*splits[indexCount - 1]);
		slot->Init();
		Items[slotNumber] = slot;	
	}

	return true;
}

void UInventoryUI::RefreshUI()
{

	UInventoryManager* Inven = GameInstance->GetInventory();
	if (IsValid(Inven) == false)
		return;
	//TList<AMyItem*> items = Inven->Items.Val
	//인벤토리 안에 있는 정보에 따라 Slot의 이미지를 바꿔주자
	for (auto item : Items)
	{
		item->Item.Clear();
		item->ItemData.ItemType = None;
		item->RefreshUI();
	}


	//인벤토리 매니저에서 저장하고 있는 데이터를 바탕으로 Update.
	TArray<TPair<int32, Protocol::ItemInfo>> array =  Inven->Items.Array();

	for (auto item : array)
	{
		Items[item.Key]->SetItem(item.Value);
		Items[item.Key]->RefreshUI();
	}

	FString gold = FString::Printf(TEXT("%d"), Gold);
	GoldText->SetText(FText::FromString(gold));
}
