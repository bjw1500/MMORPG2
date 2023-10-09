// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Network/Protocol.pb.h"
#include "Object/GameStruct.h"
#include "ItemSlot.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()
public:

	void Init();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClicked();
	void UseItem();

	void SetItem(Protocol::ItemInfo ItemInfo);
	void RefreshUI();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UImage* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UTextBlock* Text;

	UPROPERTY()
	FItemData ItemData;
	Protocol::ItemInfo Item;
};
