// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerUI.h"
#include "StatComponent.h"
#include "Network/GameManager.h"
#include "Components/ProgressBar.h"

void UPlayerUI::BindHp(UStatComponent* stat)
{
	CurrentStat = stat;
	UpdateHP();
	CurrentStat.Get()->OnHpChanged.AddUObject(this, &UPlayerUI::UpdateHP);
}

void UPlayerUI::UpdateHP()
{
	if (CurrentStat.IsValid() == true)
	{
		HPBar->SetPercent(CurrentStat->GetHpRatio());
	}

}
