// Fill out your copyright notice in the Description page of Project Settings.


#include "HpBarWidget.h"
#include "StatComponent.h"
#include "Components/ProgressBar.h"

void UHpBarWidget::BindHp(UStatComponent* stat)
{
	CurrentStat = stat;
	CurrentStat.Get()->OnHpChanged.AddUObject(this, &UHpBarWidget::UpdateHP);

}

void UHpBarWidget::UpdateHP()
{
	if (CurrentStat.IsValid() == true)
	{

		HP_BAR->SetPercent(CurrentStat->GetHpRatio());

	}


}
