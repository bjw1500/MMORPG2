// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimInstance.h"
#include "Creature.h"
#include "Network/GameManager.h"
#include "GameFramework/PawnMovementComponent.h"

UMyAnimInstance::UMyAnimInstance()
{
	static ConstructorHelpers::FObjectFinder<UAnimMontage> am(
		TEXT("/Game/Animations/Montage/SK_Mannequin_Montage.SK_Mannequin_Montage"));
	if (am.Succeeded())
	{

		AttackMontage = am.Object;

	}

}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn))
	{
		IsRunning = Pawn->GetVelocity().Size();

		auto Character = Cast<ACreature>(Pawn);
		if (IsValid(Character))
		{
			IsFalling = Character->GetMovementComponent()->IsFalling();

		}

	}


}

void UMyAnimInstance::PlayAttackMontage()
{

		Montage_Play(AttackMontage, 1.0f);

}

void UMyAnimInstance::AnimNotify_AttackHit()
{

	Utils::DebugLog(TEXT("AnimNotiy Attack Hit"));
	OnAttackHit.Broadcast();


	//Attack Check 해주기.

}
