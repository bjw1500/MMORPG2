// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/DataTableManager.h"

void UDataTableManager::Init()
{
	StatTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/StatTable.StatTable'")));
	if (IsValid(StatTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("스탯 테이블 로드 실패"));
	}

	ItemTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/ItemTable.ItemTable'")));
	if (IsValid(ItemTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("아이템 데이터 테이블 로드 실패"));
	}

	MonsterTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/MonsterTable.MonsterTable'")));
	if (IsValid(MonsterTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("몬스터 데이터 테이블 로드 실패"));
	}

	CharacterTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/CharacterTable.CharacterTable'")));
	if (IsValid(CharacterTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("캐릭터 데이터 테이블 로드 실패"));
	}

	SkillTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/SkillTable.SkillTable'")));
	if (IsValid(SkillTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("스킬 데이터 테이블 로드 실패"));
	}

	QuestTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Script/Engine.DataTable'/Game/Data/QuestTable.QuestTable'")));
	if (IsValid(QuestTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("퀘스트 데이터 테이블 로드 실패"));
	}
}

FStatData* UDataTableManager::GetStat(int32 level)
{
	if (IsValid(StatTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("스탯 데이터 테이블이 없습니다."));
		return nullptr;
	}
	else
	{
	}

	return StatTable->FindRow<FStatData>(*FString::FromInt(level), TEXT(""));
}

FItemData* UDataTableManager::GetItemData(int32 id)
{
	if (IsValid(ItemTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("아이템 데이터 테이블이 없습니다."));
		return NULL;
	}

	return ItemTable->FindRow<FItemData>(*FString::FromInt(id), TEXT(""));
}

FMonsterData* UDataTableManager::GetMonsterData(int32 id)
{
	if (IsValid(MonsterTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("몬스터 데이터 테이블이 없습니다."));
		return NULL;
	}

	return MonsterTable->FindRow<FMonsterData>(*FString::FromInt(id), TEXT(""));
}

FCharacterData* UDataTableManager::GetCharacterData(int32 id)
{
	if (IsValid(CharacterTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("캐릭터 데이터 테이블이 없습니다."));
		return NULL;
	}

	return CharacterTable->FindRow<FCharacterData>(*FString::FromInt(id), TEXT(""));
}

FSkillData* UDataTableManager::GetSkillData(FString SkillName)
{
	if (IsValid(SkillTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("스킬 데이터 테이블이 없습니다."));
		return NULL;
	}

	return SkillTable->FindRow<FSkillData>(*SkillName, TEXT(""));
}

FQuestData* UDataTableManager::GetQuestData(int32 Id)
{
	if (IsValid(QuestTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("퀘스트 데이터 테이블이 없습니다."));
		return NULL;
	}

	return QuestTable->FindRow<FQuestData>(*FString::FromInt(Id), TEXT(""));
}
