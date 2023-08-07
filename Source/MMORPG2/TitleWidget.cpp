// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleWidget.h"
#include "Network/GameManager.h"
#include "Managers/NetworkManager.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Network/ClientPacketHandler.h"


//함수 사용은 BP에서 사용 중.

bool UTitleWidget::TryConnectToServer()
{
	if (CurrentState != TitleState::Connect)
		return false;

	FString serverIP = IP_EditableText->GetText().ToString();
	wstring ip = TCHAR_TO_WCHAR(*serverIP);
	bool connect = GameInstance->GetNetworkManager()->Init(ip);

	FString test = FString::Printf(TEXT("%s"), ip.c_str());

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, *test);
	//만약 실패하면?
	return connect;
}

bool UTitleWidget::TryConnectToLogin()
{
	if (CurrentState != TitleState::Login)
		return false;

	FString id = ID_EditableText->GetText().ToString();
	FString password = Password_EditableText->GetText().ToString();

	GameInstance->GetPacketHandler()->Make_C_TryLogin(id, password);


	return true;
}

bool UTitleWidget::TryCreateAccount()
{
	if (CurrentState != TitleState::CreateAccount)
		return false;

	FString id = CreateID_EditableText->GetText().ToString();
	FString password = CreatePassword_EditableText->GetText().ToString();

	GameInstance->GetPacketHandler()->Make_C_CreateAccount(id, password);
	return true;
}
