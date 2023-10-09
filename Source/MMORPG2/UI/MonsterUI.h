// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MonsterUI.generated.h"

/**
 * 
 */
class AMonster;

UCLASS()
class MMORPG2_API UMonsterUI : public UUserWidget
{
	GENERATED_BODY()
public:
	
	void BindMonster(AMonster* target);
	void UnBind();
	void UpdateHP();

private:
	TWeakObjectPtr<class UStatComponent> CurrentStat;
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* MonsterNameText;

	AMonster* Master;
	
};
