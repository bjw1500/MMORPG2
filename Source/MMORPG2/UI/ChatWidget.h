// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	
	UFUNCTION(BlueprintCallable, Category = "Server")
	void UpdateChat(UPARAM(ref) FString& msg);
	UFUNCTION(BlueprintCallable, Category = "Server")
	void SendChat();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void SendChatByRef(UPARAM(ref) FString& msg);

	void ShowChat();


public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UEditableText* MessageText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UScrollBox* MessageList;
	
	UPROPERTY()
		TSubclassOf<class UUserWidget> ChatMessageClass;

};
