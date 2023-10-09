// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Object/GameStruct.h"
#include "Network/Protocol.pb.h"
#include "ShopItemList.generated.h"

/**
 * 
 */

class UShopItemSlot;
class UListView;

UCLASS()
class MMORPG2_API UShopItemList : public UUserWidget
{
	GENERATED_BODY()
public:

	void RefreshUI();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UListView* ShopItemList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopItemSlot> ListElement;

	UPROPERTY(EditAnywhere)
	TArray<FItemData> ItemList;
};
