// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemSlot.h"
#include "Network/GameManager.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Network/ClientPacketHandler.h"

void UShopItemSlot::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	const UShopItemSlot* Item = Cast<UShopItemSlot>(ListItemObject);

	Master = Item->Master;
	SetElement(Item->Data);
}

void UShopItemSlot::ClickedButton()
{
	GameInstance->GetPacketHandler()->Make_C_BuyItem(Data.Id);
}

void UShopItemSlot::SetElement(FItemData ItemData)
{
	Data = ItemData;

	ItemImage->SetBrushFromTexture(Data.Icon);

	ItemNameText->SetText(FText::FromString(Data.Name));

	FString price = FString::Printf(TEXT("%d"), Data.Price);
	PriceText->SetText(FText::FromString(price));

}
