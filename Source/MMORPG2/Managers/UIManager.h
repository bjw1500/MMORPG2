// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include "UIManager.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UUIManager : public UObject
{
public:
	GENERATED_BODY()

		void Init();

	void SetMainUI(UUserWidget* CurrentUI) { MainUI = CurrentUI; };
	UUserWidget* GetMainUI() { 
			return MainUI; 
	};

public:
	UPROPERTY()
	UUserWidget* PopUI;
private:
	
	UPROPERTY()
	UUserWidget* MainUI;


};
