// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NPCUI.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UNPCUI : public UUserWidget
{
	GENERATED_BODY()
public:

		UFUNCTION(BlueprintCallable, Category = "Server")
		void OnClickedShopButton();

		UFUNCTION(BlueprintCallable, Category = "Server")
		void OnClickedBossSpawnButton();

		UFUNCTION(BlueprintCallable, Category = "Server")
		void OnClickedQuestButton();
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UButton* ShopButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UButton* BossSpawnButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UButton* QuestButton;

	UPROPERTY()
	class ANPC* Master;

};
