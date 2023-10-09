// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Network/Protocol.pb.h"
#include "CharacterListElement.generated.h"

/**
 * 리스뷰에 넣기 위해서는 IUserObjectListEntry를 상복 받아줘야 한다.
 */

class UTextBlock;
class UButton;
class UCharacterSelectListUI;

UCLASS()
class MMORPG2_API UCharacterListElement : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	//함수 사용은 블루프린트에서

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	void SetElement(Protocol::CharacterListElement Info);

	UFUNCTION(BlueprintCallable, Category = "Server")
	void SelectCharacter();

	UFUNCTION(BlueprintCallable, Category = "Server")
	void ClieckedThisSlot();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* NameText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* ConnectButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UButton* SlotButton;
	UPROPERTY()
	UCharacterSelectListUI* Master;
private:

	Protocol::CharacterListElement Element;
};
