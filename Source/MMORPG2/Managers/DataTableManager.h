// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StatComponent.h"
#include "Object/GameStruct.h"
#include "DataTableManager.generated.h"

/**
 * 
 */
class UDataTable;

UCLASS()
class MMORPG2_API UDataTableManager : public UObject
{
	GENERATED_BODY()
public:

	void Init();
	FStatData* GetStat(int32 level);
	FItemData* GetItemData(int32 id);
	FMonsterData* GetMonsterData(int32 id);
	FCharacterData* GetCharacterData(int32 id);
	FSkillData* GetSkillData(FString SkillName);
	FQuestData* GetQuestData(int32 Id);


private:
	UPROPERTY();
	TObjectPtr<UDataTable> StatTable;
	UPROPERTY();
	TObjectPtr<UDataTable> MonsterTable;
	UPROPERTY();
	TObjectPtr<UDataTable> ItemTable;
	UPROPERTY();
	TObjectPtr<UDataTable> CharacterTable;
	UPROPERTY();
	TObjectPtr<UDataTable> SkillTable;
	UPROPERTY();
	TObjectPtr<UDataTable> QuestTable;
};
