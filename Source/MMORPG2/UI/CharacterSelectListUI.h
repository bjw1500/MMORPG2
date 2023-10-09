// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Network/Protocol.pb.h"
#include "CharacterSelectListUI.generated.h"


/**
 * 
 */
class UCharacterListElement;
class ULobbyUI;
class UListView;
class UButton;
class ARPGPlayer;


UCLASS()
class MMORPG2_API UCharacterSelectListUI : public UUserWidget
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Server")
	void RefreshUI();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void SetSelectedSlot(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void ClickedCreateCharacterButton();

	void SpawnCharacter(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void CreateCharacter(int32 Index, FString Name);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void SpawnSlotCharacter(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void DeleteCharacter(int32 Index);

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UListView* CharacterListView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* CharacterCreateButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* CharacterDeleteButton;
	UPROPERTY(EditAnywhere)
	ULobbyUI* Lobby;

	TArray<Protocol::CharacterListElement> CharacterInfos;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterListElement> ListElement;



private:
	UPROPERTY()
	TObjectPtr<ARPGPlayer> SelectedCharacter;

	UPROPERTY()
	int32 SelectedSlot = 10;
};
