// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/LobbyGameMode.h"
#include "Network/GameManager.h"
#include "UI/LobbyUI.h"
#include "Managers/UIManager.h"
#include "UI/CharacterSelectListUI.h"
#include "Network/ClientPacketHandler.h"

ALobbyGameMode::ALobbyGameMode()
{
	static ConstructorHelpers::FClassFinder<ULobbyUI> LobbyUI(TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/Blueprints/UI/WBP_LobbyUI.WBP_LobbyUI_C'"));
	if (LobbyUI.Succeeded())
	{
		UIClass = LobbyUI.Class;
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::Init()
{
	Super::Init();
	{
		if (IsValid(UIClass) == false)
			return;
		MainUI = CreateWidget<ULobbyUI>(GetWorld(), UIClass);
		MainUI->AddToViewport();
		GameInstance->GetUIManager()->SetMainUI(MainUI);

		ULobbyUI* lobby = Cast<ULobbyUI>(MainUI);
		lobby->CharacterList->Lobby = lobby;
		GameInstance->GetPlayerController()->SetInputMode(FInputModeGameAndUI());
		GameInstance->GetPlayerController()->SetShowMouseCursor(true);
	}


	//패킷 요청하기
	GameInstance->GetPacketHandler()->Make_C_LoadingCharacterList();
}
