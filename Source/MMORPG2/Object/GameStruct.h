// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Creature.h"
#include "Network/Protocol.pb.h"
#include "GameStruct.generated.h"


USTRUCT(Atomic, BlueprintType)
struct FStatData : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Level;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 MaxHp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float TotalExp;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float Attack;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SearchRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Skills;

};

USTRUCT(Atomic, BlueprintType)
struct FRewardData : public FTableRowBase
{
	GENERATED_BODY()

		//아이템 드랍 확률
		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		float ItemDropRate;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Id;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString ItemName;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Count;
};

USTRUCT(Atomic, BlueprintType)
struct FMonsterData : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Id;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MonsterName;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FStatData Stat;

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TArray<FRewardData> RewardDatas;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ACreature> Model;
};

UENUM()
enum EItemType
{
	WEAPON,
	ARMOR,
	COUNSUMABLE,
	None,
};

USTRUCT(Atomic, BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Armor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EItemType> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class AMyItem> Model;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;

};

UENUM()
enum SkillType
{
	Dash,
	SKILL_1,
	SKILL_2,
	SKILL_3,
};

USTRUCT(Atomic, BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Name;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<SkillType> SkillType;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 CoolTime;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Damage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		TSubclassOf<class USkillBase> SkillObject;
};

USTRUCT(Atomic, BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnyWhere, BlueprintReadWrite)
		int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FStatData Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ARPGPlayer> Model;
};


//퀘스트
USTRUCT(Atomic, BlueprintType)
struct FQuestData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	int32 Id;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TargetMonsterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetTemplatedId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TargetNumbers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CurrentProgress;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACreature> Model;

};