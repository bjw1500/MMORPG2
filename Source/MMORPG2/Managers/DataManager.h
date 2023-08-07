// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../StatComponent.h"

/**
 * 
 */
class MMORPG2_API DataManager
{
public:
	DataManager();
	~DataManager();

	void Init();

	FStatData* GetStat(int32 level);


private:
	class UDataTable* StatTable;
};
