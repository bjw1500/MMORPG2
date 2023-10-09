// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMORPG2GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API ALobbyGameMode : public AMMORPG2GameModeBase
{
	GENERATED_BODY()
	ALobbyGameMode();

	virtual void BeginPlay() override;
	void Init() override;
};
