// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "Managers/ObjectManager.h"
#include "Network/GameManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MyAnimInstance.h"
#include "Engine/DamageEvents.h"
#include "Managers/UIManager.h"
#include "MonsterController.h"
#include "Network/ClientPacketHandler.h"
#include "UI/InGameUI.h"
#include "UI/MonsterUI.h"
#include "HpBarWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"



AMonster::AMonster()
{
	//Init();


	AIControllerClass = AMonsterController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AMonster::BeginPlay()
{
	Super::BeginPlay();
	TArray<UCapsuleComponent*> ComponentArray;
	GetComponents<UCapsuleComponent>(ComponentArray);
	for (UCapsuleComponent* children : ComponentArray)
	{
		if (children->GetFName() == "MonsterWeaponCollision")
		{
			WeaponCollision = children;
			//WeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AMonster::OnWeaponOverlap);
			//WeaponCollision->OnComponentEndOverlap.AddDynamic(this, &AMonster::OnWeaponOverlapEnd);
			break;
		}
	}


}

void AMonster::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AMonster::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GameInstance->bConnected == false)
		return;


	UpdateController();
}

void AMonster::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) == true)
	{
		if (OtherActor ==  this)
			return;
		Utils::DebugLog(TEXT("OnWeaponOverlap"));
		Protocol::Stat infoStat = GetInfo()->stat();
		FDamageEvent damageEvent;
		UGameplayStatics::ApplyDamage(OtherActor, Stat->GetDamage(), GetController(), this, nullptr);
		//OtherActor->TakeDamage(Stat->GetDamage(), damageEvent, GetController(), this);
		WeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	}
}

void AMonster::OnWeaponOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}



float AMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	return DamageAmount;
}

void AMonster::OnDead()
{
	Anim->SetState(Protocol::CreatureState::Dead);
	UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
	UI->MonsterUI->UnBind();
	//GetCapsuleComponent()->SetCollisionProfileName(TEXT("Item"));
}

void AMonster::RotationToTarget()
{
		ACreature* target= GameInstance->GetObjectManager()->GetPlayerByID(GetInfo()->targetid());

		if (IsValid(target) == false)
		{
			//target이 없다면,
			if (IsTracking == true && GetMonsterData()->MonsterType == EMonsterType::Boss)
			{
				UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
				UI->MonsterUI->UnBind();
				IsTracking = false;
			}
			else if (IsTracking  == true && GetMonsterData()->MonsterType == EMonsterType::Normal)
			{
				UHpBarWidget* hpBar = Cast<UHpBarWidget>(HpBar->GetUserWidgetObject());
				FMonsterData* data = GetMonsterData();
				if (data == nullptr)
					return;
					hpBar->SetVisibility(ESlateVisibility::Hidden);
				IsTracking = false;
			}
			return;
		}

		if (IsTracking == false &&GetMonsterData()->MonsterType == EMonsterType::Boss)
		{
			//타깃이 존재한다면,
			UInGameUI* UI = Cast<UInGameUI>(GameInstance->GetUIManager()->GetMainUI());
			UI->MonsterUI->BindMonster(this);
			IsTracking = true;
		}
		else if (IsTracking == false && GetMonsterData()->MonsterType == EMonsterType::Normal)
		{
			UHpBarWidget* hpBar = Cast<UHpBarWidget>(HpBar->GetUserWidgetObject());
			FMonsterData* data = GetMonsterData();
			if (data == nullptr)
				return;
			hpBar->SetVisibility(ESlateVisibility::Visible);
			IsTracking = true;
		}

		//FRotator look = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target->GetActorLocation());
		//look.Pitch = 0;
		//SetActorRelativeRotation(look);
}

void AMonster::SyncPos()
{
	const auto& location = GetActorLocation();
	const auto& rotation = GetActorRotation();
	const auto& velocity = GetVelocity();

	Protocol::ObjectInfo newInfo;
	newInfo.CopyFrom(*GetInfo());
	Protocol::Position* updatePosition = newInfo.mutable_position();

	updatePosition->set_locationx(location.X);
	updatePosition->set_locationy(location.Y);
	updatePosition->set_locationz(location.Z);

	updatePosition->set_rotationx(rotation.Roll);
	updatePosition->set_rotationy(0);
	updatePosition->set_rotationz(rotation.Yaw);

	updatePosition->set_velocityx(velocity.X);
	updatePosition->set_velocityy(velocity.Y);
	updatePosition->set_velocityz(velocity.Z);

	//서버와 연결된 상태가 아니라면, 
	if (GameInstance->bConnected == false)
		return;

	IsSync = true;
	GameInstance->GetPacketHandler()->Make_C_MonsterMove(newInfo);
}

void AMonster::UpdateInfo(Protocol::ObjectInfo* info)
{
	if (IsSync == false)
	{
		SyncPos();
		return;
	}

	Info.CopyFrom(*info);
	bool IsFalling = GetCharacterMovement()->IsFalling();
	if (IsFalling == true)
	{
		SyncPos();
		return;
	}

	FVector location;
	location.X = Info.position().locationx();
	location.Y = Info.position().locationy();
	location.Z = Info.position().locationz();

	FRotator rotation;
	rotation.Roll = Info.position().rotationx();
	rotation.Pitch = Info.position().rotationy();
	rotation.Yaw = Info.position().rotationz();

	RotationToTarget();
	FVector velocity;
	velocity.X = Info.position().velocityx();
	velocity.Y = Info.position().velocityy();
	velocity.Z = Info.position().velocityz();
	GetCharacterMovement()->Velocity = velocity;

	//TODO Velocity 추가
	SetActorLocationAndRotation(location, rotation);
	//SetActorRelativeLocation(location);
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), location);
	Horizontal = GetCharacterMovement()->Velocity.X;
	Vertical = GetCharacterMovement()->Velocity.Y;

}


void AMonster::UpdateController()
{
	//FString text = FString::Printf(TEXT("%f"), GetInfo()->stat().damage());
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, *text);

	switch (GetInfo()->state())
	{
	case Protocol::CreatureState::Idle:
		UpdateIdle();
		break;
	case Protocol::CreatureState::Move:
		UpdateMove();
		break;
	case Protocol::CreatureState::Attack:
		UpdateAttack();
		break;
	case Protocol::CreatureState::Dead:
		UpdateDead();
		break;
	case Protocol::CreatureState::Casting:
		UpdateCasting();
		break;
	default:
		break;
	}


}

void AMonster::UpdateIdle()
{
}

void AMonster::UpdateMove()
{



}

void AMonster::UpdateAttack()
{
	



}

void AMonster::UpdateDead()
{
}

void AMonster::UpdateCasting()
{


}


