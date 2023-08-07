// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


#include "GameFramework/GameModeBase.h"
#include "CoreMinimal.h"
#include "Network/GameManager.h"
#include "Network/NetworkSession.h"
#include "MMORPG2GameModeBase.generated.h"


UCLASS()
class MMORPG2_API AMMORPG2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
		virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI");
	class UUserWidget* Main_UI;
	
};
