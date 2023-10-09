// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterListElement.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Network/ClientPacketHandler.h"
#include "Network/GameManager.h"
#include "Components/ListView.h"
#include "UI/CharacterSelectListUI.h"

void UCharacterListElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	const UCharacterListElement* Item = Cast<UCharacterListElement>(ListItemObject);

	Master = Item->Master;

	int32 level = Item->Element.playerinfo().stat().level();
	FString name = UTF8_TO_TCHAR(Item->Element.playerinfo().name().c_str());
	FString text = FString::Printf(TEXT("Lv.%d %s"), level, *name);
	NameText->SetText(FText::FromString(*text));
	SetElement(Item->Element);
}

void UCharacterListElement::SetElement(Protocol::CharacterListElement Info)
{
	Element = Info;
}

void UCharacterListElement::SelectCharacter()
{
	GameInstance->LoadGameLevel(FString(TEXT("Game")));
	FTimerHandle WaitHandle;
	int32 slot = Element.slot();
	GameInstance->GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([=]()
		{
			GameInstance->GetPacketHandler()->Make_C_EnterRoom(slot);

		}), 1, false);
}

void UCharacterListElement::ClieckedThisSlot()
{
	if (IsValid(Master) == false)
	{
		Utils::DebugLog(TEXT("ClieckedThisSlot Error"));
		return;
	}

	Master->SetSelectedSlot(Element.slot());
}

