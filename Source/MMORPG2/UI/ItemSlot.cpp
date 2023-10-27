// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ItemSlot.h"
#include "Object/MyItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Network/GameManager.h"
#include "Managers/DataTableManager.h"
#include "Network/ClientPacketHandler.h"
#include "Managers/ObjectManager.h"

void UItemSlot::Init()
{
	UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Script/Engine.Texture2D'/Game/Texture/White.White'")));
	Icon->SetBrushFromTexture(Texture, true);
}

void UItemSlot::OnClicked()
{
	//아무런 아이템이 없는 빈 슬롯이라면, 
	if (IsValid(ItemData.Model) == false)
		return;

	UseItem();
}

void UItemSlot::UseItem()
{
	switch (ItemData.ItemType)
	{
	case WEAPON:
	{
		Protocol::ObjectInfo* info = GameInstance->GetObjectManager()->GetMyPlayer()->GetInfo();
		GameInstance->GetPacketHandler()->Make_C_EquippedItem(*info, Item);
		break;
	}
	case ARMOR:

		break;
	case COUNSUMABLE:
	{
		//포션 사용 요청 패킷을 보낸다.
		Protocol::ObjectInfo* info = GameInstance->GetObjectManager()->GetMyPlayer()->GetInfo();
		GameInstance->GetPacketHandler()->Make_C_UseItem(*info, Item);
		break;
	}
	default:
		break;
	}
}

void UItemSlot::SetItem(Protocol::ItemInfo ItemInfo)
{
	Item = ItemInfo;

	FItemData* data = GameInstance->GetDataManager()->GetItemData(Item.templateid());
	if (data == nullptr)
		return;
	ItemData = *data;
	
	//Texture 정보를 바꿔준다.
	UTexture2D* Texture = ItemData.Icon;
	if (IsValid(Texture) == false)
	{
		Utils::DebugLog(TEXT("Set Item Error. 텍스쳐 정보가 없습니다."));
		return;
	}

	Icon->SetBrushFromTexture(Texture, true);
}

void UItemSlot::RefreshUI()
{
	//아이템이 없는 빈 슬롯이라면,
	if (ItemData.ItemType == None)
	{
		UTexture2D* Texture = Cast<UTexture2D>(StaticLoadObject(UTexture2D::StaticClass(), NULL, TEXT("/Script/Engine.Texture2D'/Game/Texture/White.White'")));
		Icon->SetBrushFromTexture(Texture, true);
		Text->SetText(FText::FromString(TEXT(" ")));
		return;
	}

	UTexture2D* Texture = ItemData.Icon;
	if (IsValid(Texture) == false)
	{
		Utils::DebugLog(TEXT("Refresh UI Error. 텍스쳐 정보가 없습니다."));
		return;
	}
	Icon->SetBrushFromTexture(Texture, true);
	if(Item.isequipped() == 1)
		Text->SetText (FText::FromString(TEXT("장착 중")));
	else
		Text->SetText (FText::FromString(TEXT(" ")));
}
