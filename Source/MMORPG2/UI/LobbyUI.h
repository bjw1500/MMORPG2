// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUI.generated.h"

/**
 * 
 */
class UCharacterSelectListUI;
class UButton;
class UEditableText;

UCLASS()
class MMORPG2_API ULobbyUI : public UUserWidget
{
	GENERATED_BODY()
public:

	void ChangeCreateCharacterMode();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClickedButton1();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClickedButton2();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClickedButton3();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClickedOKButton();
	UFUNCTION(BlueprintCallable, Category = "Server")
	void OnClickedCancelButton();


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UCharacterSelectListUI* CharacterList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* IndexButton1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* IndexButton2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* IndexButton3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UEditableText* NameEditableText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* OKButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* CancelButton;

public:
	int32 SelectIndex = 1;
};
