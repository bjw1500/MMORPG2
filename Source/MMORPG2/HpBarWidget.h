// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpBarWidget.generated.h"

/**
 * 
 */



UCLASS()
class MMORPG2_API UHpBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void BindHp(class UStatComponent* stat);
	void UpdateHP();



private:

	TWeakObjectPtr<class UStatComponent> CurrentStat;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HP_BAR;
};
