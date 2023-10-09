// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MMORPG2GameModeBase.h"
#include "InGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API AInGameMode : public AMMORPG2GameModeBase
{
public:
	GENERATED_BODY()
	AInGameMode();

	virtual void BeginPlay() override;
	void Init() override;
	
};
