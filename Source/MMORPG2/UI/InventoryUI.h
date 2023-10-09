// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryUI.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UInventoryUI : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	void RefreshUI();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UGridPanel* GridSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UTextBlock* GoldText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Meta = (BindWidget))
	class UButton* DragButton;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (AllowPrivateAccess = True))
	TArray<class UItemSlot*> Items;

	UPROPERTY()
	int32 Gold;
};
