// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Network/GameManager.h"
#include "TitleWidget.generated.h"

/**
 * 
 */


//타이틀에서는 서버 접속에 필요한 IP 주소와 플레이어 아이디를 받아줄 것이다.
UENUM(BlueprintType)
enum class TitleState : uint8
{
	Connect,
	Login,
	CreateAccount,
	LoginSuccess
};


UCLASS()
class MMORPG2_API UTitleWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable, Category = "Server")
		bool TryConnectToServer();

	UFUNCTION(BlueprintCallable, Category = "Server")
		bool TryConnectToLogin();

	UFUNCTION(BlueprintCallable, Category = "Server")
		bool TryCreateAccount();



public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (BindWidget))
		class UEditableText* IP_EditableText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (BindWidget))
		class UEditableText* ID_EditableText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (BindWidget))
		class UEditableText* Password_EditableText;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UEditableText* CreateID_EditableText;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UEditableText* CreatePassword_EditableText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (BindWidget))
		class UButton* Check_Button;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UButton* CreateAccount_Button;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
		class UTextBlock* Information_Text;


	UPROPERTY()
		FString ServerIP;
	UPROPERTY()
		FString UserId;
	UPROPERTY()
		FString UserPassword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TitleState CurrentState = TitleState::Connect;



};
