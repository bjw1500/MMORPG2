// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ListView.h"
#include "UI/QuestListElement.h"
#include "Object/GameStruct.h"
#include "QuestUI.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UQuestUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void RefreshUI();
	void AddQuest(int32 TemplatedId, int32 Progress);
	FQuestData* GetQuestData(int32 TemplatedId);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UListView* QuestList;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UQuestListElement> QuestElement;

	UPROPERTY()
	TArray<UQuestListElement*> ElementList;
};
