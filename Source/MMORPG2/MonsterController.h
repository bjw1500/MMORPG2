// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MonsterController.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API AMonsterController : public AAIController
{
	GENERATED_BODY()
public:
	AMonsterController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;


};
