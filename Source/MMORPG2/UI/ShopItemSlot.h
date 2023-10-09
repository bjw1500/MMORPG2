// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Object/GameStruct.h"
#include "ShopItemSlot.generated.h"

/**
 * 
 */

class UButton;
class UTextBlock;
class UImage;
class UShopItemList;

UCLASS()
class MMORPG2_API UShopItemSlot : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION(BlueprintCallable, Category = "Server")
	void ClickedButton();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void SetElement(FItemData ItemData);


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* BuyButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UImage* ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ItemNameText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* PriceText;

	UPROPERTY()
	FItemData Data;

	UPROPERTY()
	UShopItemList* Master;

};
