// Copyright Epic Games, Inc. All Rights Reserved.

#include "MMORPG2GameModeBase.h"
#include "Network/SocketUtils.h"
#include "Network/Service.h"
#include "Network/NetAddress.h"
#include "Network/Protocol.pb.h"
#include "Managers/ObjectManager.h"
#include "Managers/NetworkManager.h"
#include "Network/GameManager.h"


void AMMORPG2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	Utils::DebugLog(TEXT("GameMode::Begin Play"));


}
