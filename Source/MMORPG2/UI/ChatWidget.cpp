// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatWidget.h"
#include "Components/EditableText.h"
#include "Components/MultiLineEditableText.h"
#include "Network/GameManager.h"
#include "Network/ClientPacketHandler.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "ChatMessage.h"


void UChatWidget::UpdateChat(FString& msg)
{
	if (IsValid(ChatMessageClass) == false)
	{
		ChatMessageClass = StaticLoadClass(UChatMessage::StaticClass(), NULL, TEXT("/Game/Blueprints/UI/WBP_ChatMessage.WBP_ChatMessage_C"));
		if (IsValid(ChatMessageClass) == false)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("Chat Message Class 로드 실패"));
			return;
		}
	}

	UChatMessage* newChat = Cast<UChatMessage>(CreateWidget(GetWorld(), ChatMessageClass));
	newChat->ChatText->SetText(FText::FromString(msg));
	MessageList->AddChild(newChat);
}

void UChatWidget::SendChat()
{
	FString msg = MessageText->GetText().ToString();

	if(GameInstance->bConnected == true)
		GameInstance->GetPacketHandler()->Make_C_Chat(msg);
	
	FText empty = FText::FromString(TEXT(""));
	MessageText->SetText(empty);
}

void UChatWidget::ShowChat()
{

	GameInstance->GetPlayerController()->SetInputMode(FInputModeGameAndUI());

}

