// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "Object/Creature.h"
#include "Network/GameManager.h"
#include "Network/Protocol.pb.h"
#include "Network/ClientPacketHandler.h"
#include "Blueprint/UserWidget.h"
#include "UI/InGameUI.h"
#include "Managers/UIManager.h"
#include "UI/PlayerUI.h"
#include "MyAnimInstance.h"
#include "Skill/SkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetInfo();
}

void UStatComponent::SetInfo()
{
	ACreature* master = Cast<ACreature>(GetOwner());
	MasterInfo = master->GetInfo();
	Master = master;
	Master->Skill->SetInfo();
	Master->GetCharacterMovement()->MaxWalkSpeed = GetSpeed();
	OnHpChanged.Broadcast();
}

void UStatComponent::BindPlayerUI()
{
	//Main UI를 찾아서 연결한다.
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	if (IsValid(UI) == false)
		return;

	UI->PlayerUI->BindHp(this);


}

void UStatComponent::OnDamaged(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float hp = GetHp() - DamageAmount;

	if (hp <= 0)
	{

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("캐릭터가 사망했습니다"));

	}


	//서버와 연결이 되었는가?
	if(GameInstance->bConnected == false)
		SetHp(hp);
	else
	{
		if (Master->GetInfo()->type() == Protocol::ObjectType::PLAYER &&Master->ThisMasterOtherClient == true)
			return;

		ACreature* causer = Cast<ACreature>(DamageCauser);
		GameInstance->GetPacketHandler()->Make_C_ChangedHP(*MasterInfo, *causer->GetInfo(),(int32) DamageAmount);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Make Changed Packet"));
	}

}

void UStatComponent::SetHp(int32 value)
{
	int32 hp = FMath::Clamp(value, 0, GetStat()->maxhp());
	if (hp <= 0)
	{

		//캐릭터 사망.
		if (GameInstance->bConnected == false)
			Master->OnDead();

	}
	MasterInfo->mutable_stat()->set_hp(hp);
	FString string = FString::Printf(TEXT("현재 체력 %f"), GetHp());
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *string);
	OnHpChanged.Broadcast();
}



