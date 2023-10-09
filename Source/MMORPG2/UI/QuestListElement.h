// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/MultiLineEditableText.h"
#include "Object/GameStruct.h"
#include "QuestListElement.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UQuestListElement : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void RefreshUI();
	void SetElement(FQuestData NewQuestData);


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* QuestNameText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		UMultiLineEditableText* QuestDescriptionText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* TargetNumbersText;

	FQuestData Data;
	UPROPERTY()
	class UQuestUI* Master;
	
};
