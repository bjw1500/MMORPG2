// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MonsterUI.h"
#include "Object/Monster.h"
#include "Network/GameManager.h"
#include "Managers/DataTableManager.h"
#include "Components/ProgressBar.h"
#include "Object/GameStruct.h"
#include "Components/TextBlock.h"



void UMonsterUI::BindMonster(AMonster* target)
{
	if (IsValid(target) == false)
		return;
	Master = target;
	CurrentStat = target->Stat;
	UpdateHP();
	CurrentStat.Get()->OnHpChanged.AddUObject(this, &UMonsterUI::UpdateHP);
	SetVisibility(ESlateVisibility::Visible);

	FMonsterData* data = GameInstance->GetDataManager()->GetMonsterData(Master->GetInfo()->templateid());
	MonsterNameText->SetText(FText::FromString(data->MonsterName));

}

void UMonsterUI::UnBind()
{
	if(IsValid(Master) == false)
		return;

	CurrentStat.Get()->OnHpChanged.Clear();
	Master = nullptr;
	CurrentStat = nullptr;
	SetVisibility(ESlateVisibility::Hidden);
}

void UMonsterUI::UpdateHP()
{
	if (CurrentStat.IsValid() == true)
	{
		HPBar->SetPercent(CurrentStat->GetHpRatio());
	}
}
