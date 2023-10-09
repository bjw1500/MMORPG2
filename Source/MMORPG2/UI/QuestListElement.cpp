// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QuestListElement.h"
#include "UI/QuestUI.h"
#include "Components/EditableText.h"
#include "Components/MultiLineEditableText.h"
#include "Network/GameManager.h"
#include "Network/ClientPacketHandler.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetComponent.h"
#include "Managers/DataTableManager.h"
#include "Components/TextBlock.h"

void UQuestListElement::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	UQuestListElement* Element = Cast<UQuestListElement>(ListItemObject);

	Master = Element->Master;
	SetElement(Element->Data);
	Master->ElementList.Add(this);
	RefreshUI();
}

void UQuestListElement::RefreshUI()
{
	QuestNameText->SetText(Data.QuestName);
	QuestDescriptionText->SetText(Data.Description);

	//퀘스트의 진행도는 서버에서 저장해야 하나?
	int32 currentProgress = Data.CurrentProgress;
	int32 numbers = Data.TargetNumbers;

	FString format = FString::Printf(TEXT("%s [% d / % d]"), *Data.TargetMonsterName.ToString(), currentProgress, numbers);
	FText text = FText::FromString(format);
	TargetNumbersText->SetText(text);
}

void UQuestListElement::SetElement(FQuestData NewQuestData)
{
	Data = NewQuestData;
}
