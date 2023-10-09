// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"
#include "SkillBase.h"
#include "Object/Creature.h"
#include "Network/GameManager.h"

#include "Managers/DataTableManager.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USkillComponent::SetInfo()
{
	Master = Cast<ACreature>( GetOwner());
	LoadSkills(Master->Stat->GetStat());
}

void USkillComponent::LoadSkills(Protocol::Stat* CreatureStat)
{
	for (auto skill : CreatureStat->skills())
	{
		FString text = UTF8_TO_TCHAR(skill.c_str());
		FSkillData* data=  GameInstance->GetDataManager()->GetSkillData(text);
		USkillBase* newSkill = NewObject<USkillBase>(this, data->SkillObject);
		newSkill->SetSkillData(*data);
		newSkill->SetMaster(Master);
		Skills.Add((int32)data->SkillType, newSkill);
	}
}

bool USkillComponent::ActiveSkill(SkillType Type)
{
	if (Skills.Find((int32)Type) == false)
	{
		FString ErrorMsg = FString::Printf(TEXT("%d 스킬 목록에 없는 스킬입니다."), Type);
		Utils::DebugLog(ErrorMsg);
		return false;
	}

	USkillBase* skill = Skills[(int32)Type];
	if (IsValid(skill) == false)
	{
		Utils::DebugLog(TEXT("Active Skill Null"));
		return false;
	}

	skill->Active();
	return true;
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

