// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Managers/ObjectManager.h"
#include "Network/GameManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyAnimInstance.h"

AMonster::AMonster()
{
	//Init();

}

void AMonster::Tick(float DeltaSeconds)
{


	if (GameInstance->bConnected == false)
		return;


	UpdateController();
}

float AMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Stat->OnDamaged(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	return DamageAmount;
}

void AMonster::RotationToTarget()
{
		ACreature* target= GameInstance->GetObjectManager()->GetPlayerByID(GetInfo()->targetid());
		if (IsValid(target) == false)
			return;

/*		FString text = FString::Printf(TEXT("현재 타겟 %s"), UTF8_TO_TCHAR(target->GetInfo()->name().c_str()));
		Utils::DebugLog(text)*/;
		SetActorRelativeRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target->GetActorLocation()));
}

void AMonster::UpdateInfo(Protocol::ObjectInfo* info)
{
	Info.CopyFrom(*info);

	FVector location;
	location.X = Info.position().locationx();
	location.Y = Info.position().locationy();
	location.Z = Info.position().locationz();

	RotationToTarget();

	//TODO Velocity 추가
	SetActorRelativeLocation(location);

	FVector velocity;
	velocity.X = Info.position().velocityx();
	velocity.Y = Info.position().velocityy();
	velocity.Z = Info.position().velocityz();
	GetCharacterMovement()->Velocity = velocity;

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
	default:
		break;
	}


}

void AMonster::UpdateIdle()
{
}

void AMonster::UpdateMove()
{
	if (Anim->Montage_IsPlaying(Anim->AttackMontage) == true)
	{
		//Anim->Montage_Stop(0.1f, Anim->AttackMontage);
	}
}

void AMonster::UpdateAttack()
{
}

void AMonster::UpdateDead()
{
}


