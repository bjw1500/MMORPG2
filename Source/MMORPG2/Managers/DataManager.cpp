// Fill out your copyright notice in the Description page of Project Settings.


#include "DataManager.h"

DataManager::DataManager()
{
}

DataManager::~DataManager()
{
}

void DataManager::Init()
{
	StatTable = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), NULL, TEXT("/Game/Data/DT_Stat")));
	if (IsValid(StatTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("데이터 테이블 로드 실패"));
	}

}

FStatData* DataManager::GetStat(int32 level)
{
	if (IsValid(StatTable) == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("데이터 테이블이 없습니다."));
		return NULL;
	}
	else
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("데이터 로드"));
	}

	return StatTable->FindRow<FStatData>(*FString::FromInt(level), TEXT(""));
}
