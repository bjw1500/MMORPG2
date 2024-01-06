// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "UI/InGameUI.h"
#include "Network/GameManager.h"
#include "Managers/UIManager.h"


UInGameUI* UMyBlueprintFunctionLibrary::GetUIManager()
{
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return nullptr;

	return UI;
}
