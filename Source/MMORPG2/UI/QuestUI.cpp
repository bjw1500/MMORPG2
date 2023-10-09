// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QuestUI.h"
#include "Components/EditableText.h"
#include "Components/MultiLineEditableText.h"
#include "Network/GameManager.h"
#include "Network/ClientPacketHandler.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetComponent.h"
#include "Managers/DataTableManager.h"
#include "Components/TextBlock.h"

void UQuestUI::RefreshUI()
{
	for (auto item : ElementList)
	{
		UQuestListElement* element = Cast<UQuestListElement>(item);
		element->RefreshUI();
	}
}

void UQuestUI::AddQuest(int32 TemplatedId, int32 Progress)
{
	FQuestData* newQuest = GameInstance->GetDataManager()->GetQuestData(TemplatedId);
	
	if (newQuest == nullptr)
		return;
	newQuest->CurrentProgress = Progress;

	UQuestListElement* newElement = Cast<UQuestListElement>(CreateWidget(GetWorld(), QuestElement));
	newElement->SetElement(*newQuest);
	newElement->Master = this;
	QuestList->AddItem(newElement);
}

FQuestData* UQuestUI::GetQuestData(int32 TemplatedId)
{
	for (auto item : ElementList)
	{
		UQuestListElement* element = Cast<UQuestListElement>(item);
		
		if (element->Data.Id == TemplatedId)
			return &element->Data;
	}

	return nullptr;
}
