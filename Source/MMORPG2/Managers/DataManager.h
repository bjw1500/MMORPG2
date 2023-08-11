// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../StatComponent.h"
#include "../GameStruct.h"

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
	FItemData* GetItemData(int32 id);
	FMonsterData* GetMonsterData(int32 id);


private:
	class UDataTable* StatTable;
	class UDataTable* MonsterTable;
	class UDataTable* ItemTable;
};
