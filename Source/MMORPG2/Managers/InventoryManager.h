// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Network/Protocol.pb.h"
#include "InventoryManager.generated.h"

/**
 * 
 */
class AMyItem;

UCLASS()
class MMORPG2_API UInventoryManager : public UObject
{
	GENERATED_BODY()
public:


	void AddItem(Protocol::ItemInfo* Info);
	Protocol::ItemInfo* GetItem(int ItemDbId);
	

public:
	TMap<int32, Protocol::ItemInfo> Items;
};
