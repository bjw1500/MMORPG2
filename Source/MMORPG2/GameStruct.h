// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Creature.h"
#include "MyItem.h"
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
		FRewardData Reward;

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ACreature> Model;
};

UENUM()
enum EItemType
{
	WEAPON,
	ARMOR,
	COUNSUMABLE,
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
		TEnumAsByte<EItemType> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AMyItem> Model;
};

/*
message ItemInfo
{
	int32 Id = 1;
	int32 templateId = 2;
	string name = 3;
	ItemType itemType = 4;
	int32 count = 5;
	int32 slot =6;
	int32 IsEquipped = 7;

}

*/