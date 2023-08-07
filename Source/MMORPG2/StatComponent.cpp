// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "Creature.h"
#include "Network/GameManager.h"
#include "Network/Protocol.pb.h"
#include "Network/ClientPacketHandler.h"

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
}

void UStatComponent::OnDamaged(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float hp = GetHp() - DamageAmount;
	
	FString string = FString::Printf(TEXT("%s에게서 %f만큼 데미지를 받았습니다."), *DamageCauser->GetName(), DamageAmount);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *string);

	if (hp <= 0)
	{

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("캐릭터가 사망했습니다"));

	}


	//서버와 연결이 되었는가?
	if(GameInstance->bConnected == false)
		SetHp(hp);
	else
	{
		if (Master->GetInfo()->type() != Protocol::ObjectType::MONSTER &&Master->ThisMasterOtherClient == true)
			return;

		ACreature* causer = Cast<ACreature>(DamageCauser);
		GameInstance->GetPacketHandler()->Make_C_ChangedHP(*MasterInfo, *causer->GetInfo(),(int32) DamageAmount);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Make Changed Packet"));
	}
}

void UStatComponent::SetHp(int32 value)
{
	int32 hp = FMath::Clamp(value, 0, GetStat().maxhp());
	if (hp <= 0)
	{

		//캐릭터 사망.

	}
	MasterInfo->mutable_stat()->set_hp(hp);
	FString string = FString::Printf(TEXT("현재 체력 %f"), GetHp());
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, *string);
	OnHpChanged.Broadcast();
}



