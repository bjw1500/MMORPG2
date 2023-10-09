// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMORPG2GameModeBase.h"
#include "Network/SocketUtils.h"
#include "Network/Service.h"
#include "Network/NetAddress.h"
#include "Network/Protocol.pb.h"
#include "Managers/ObjectManager.h"
#include "Managers/NetworkManager.h"
#include "UI/ChatWidget.h"
#include "Network/GameManager.h"
#include "Managers/UIManager.h"
#include "Blueprint/UserWidget.h"


AMMORPG2GameModeBase::AMMORPG2GameModeBase()
{
	//static ConstructorHelpers::FClassFinder<UUserWidget> WB_Title(TEXT("WidgetBlueprint'/Game/Blueprints/UI/WBP_Chat.WBP_Chat_C'"));
	//if (WB_Title.Succeeded())
	//{
	//	Main_UI = WB_Title.Class;
	//}

}

void AMMORPG2GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Init();


}

void AMMORPG2GameModeBase::Init()
{
}

