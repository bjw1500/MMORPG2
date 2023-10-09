// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUI.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UPlayerUI : public UUserWidget
{
public:
	GENERATED_BODY()

public:

	void BindHp(class UStatComponent* stat);
	void UpdateHP();


private:

	TWeakObjectPtr<class UStatComponent> CurrentStat;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* MPBar;
};
