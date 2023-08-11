// Fill out your copyright notice in the Description page of Project Settings.


#include "DataManager.h"
#include "../GameStruct.h"

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

void DataManager::Init()
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



}

FStatData* DataManager::GetStat(int32 level)
{
	if (IsValid(StatTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("스탯 데이터 테이블이 없습니다."));
		return nullptr;
	}
	else
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("데이터 로드"));
	}

	return StatTable->FindRow<FStatData>(*FString::FromInt(level), TEXT(""));
}

FItemData* DataManager::GetItemData(int32 id)
{
	if (IsValid(ItemTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("아이템 데이터 테이블이 없습니다."));
		return NULL;
	}

	return ItemTable->FindRow<FItemData>(*FString::FromInt(id), TEXT(""));
}

FMonsterData* DataManager::GetMonsterData(int32 id)
{
	if (IsValid(MonsterTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("아이템 데이터 테이블이 없습니다."));
		return NULL;
	}

	return MonsterTable->FindRow<FMonsterData>(*FString::FromInt(id), TEXT(""));
}
