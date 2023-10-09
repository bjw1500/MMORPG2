// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyUI.h"
#include "UI/CharacterSelectListUI.h"
#include "Components/Button.h"
#include "Components/EditableText.h"

void ULobbyUI::ChangeCreateCharacterMode()
{
	//캐릭터 만드는 UI를 띄운다.
	CharacterList->SetVisibility(ESlateVisibility::Hidden);

	IndexButton1->SetVisibility(ESlateVisibility::Visible);
	IndexButton2->SetVisibility(ESlateVisibility::Visible);
	IndexButton3->SetVisibility(ESlateVisibility::Visible);

	OKButton->SetVisibility(ESlateVisibility::Visible);
	CancelButton->SetVisibility(ESlateVisibility::Visible);
	NameEditableText->SetVisibility(ESlateVisibility::Visible);

	CharacterList->SpawnCharacter(1);
}

void ULobbyUI::OnClickedButton1()
{
	CharacterList->SpawnCharacter(1);
	SelectIndex = 1;
}

void ULobbyUI::OnClickedButton2()
{
	CharacterList->SpawnCharacter(2);
	SelectIndex = 2;
}

void ULobbyUI::OnClickedButton3()
{
	CharacterList->SpawnCharacter(3);
	SelectIndex = 3;
}

void ULobbyUI::OnClickedOKButton()
{
	FString name = NameEditableText->GetText().ToString();

	CharacterList->CreateCharacter(SelectIndex, name);
	OnClickedCancelButton();
}

void ULobbyUI::OnClickedCancelButton()
{
	CharacterList->SetVisibility(ESlateVisibility::Visible);

	IndexButton1->SetVisibility(ESlateVisibility::Hidden);
	IndexButton2->SetVisibility(ESlateVisibility::Hidden);
	IndexButton3->SetVisibility(ESlateVisibility::Hidden);

	OKButton->SetVisibility(ESlateVisibility::Hidden);
	CancelButton->SetVisibility(ESlateVisibility::Hidden);
	NameEditableText->SetVisibility(ESlateVisibility::Hidden);
}
