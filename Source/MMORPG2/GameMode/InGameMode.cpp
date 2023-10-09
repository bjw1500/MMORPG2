// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/InGameMode.h"
#include "UI/InGameUI.h"
#include "Managers/UIManager.h"

AInGameMode::AInGameMode()
{
	static ConstructorHelpers::FClassFinder<UInGameUI> WB_InGameUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_InGameUI.WBP_InGameUI_C'"));
if (WB_InGameUI.Succeeded())
{
	UIClass = WB_InGameUI.Class;
}

}

void AInGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AInGameMode::Init()
{
	Super::Init();
	{
		if (IsValid(UIClass) == false)
			return;
		MainUI = CreateWidget<UInGameUI>(GetWorld(), UIClass);
		MainUI->AddToViewport();
		GameInstance->GetUIManager()->SetMainUI(MainUI);
		GameInstance->GetPlayerController()->SetInputMode(FInputModeGameAndUI());
		GameInstance->GetPlayerController()->SetShowMouseCursor(true);
	}

}
