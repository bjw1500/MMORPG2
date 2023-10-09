// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimInstance.h"
#include "Object/Creature.h"
#include "Network/GameManager.h"
#include "Network/Protocol.pb.h"
#include "Object/Monster.h"
#include "Object/RPGPlayer.h"
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

	if (GameInstance == nullptr)
		return;

	if (GameInstance->bConnected == true)
	{
		UpdateState();
	}
	else
	{
		auto Pawn = TryGetPawnOwner();
		if (IsValid(Pawn) == false)
			return;

		ACreature* master = Cast<ACreature>(Pawn);
		if (IsValid(master) == false)
		{
			IsMoving = Pawn->GetVelocity().Size();
			IsFalling = Pawn->GetMovementComponent()->IsFalling();
		}
	}


}

void UMyAnimInstance::PlayAttackMontage()
{
	if (IsValid(AttackMontage) == false)
		return;
	if (Montage_IsPlaying(HitReactMontage))
	{
		Montage_Stop(0, HitReactMontage);
	}

	Montage_Play(AttackMontage, 1.0f);
}

void UMyAnimInstance::PlayDashMontage()
{
	if (IsValid(DashMontage) == false)
		return;
	Montage_Play(DashMontage, 1.0f);
}

void UMyAnimInstance::PlayHitReactMontage()
{
	if (IsValid(HitReactMontage) == false)
		return;
	if (Montage_IsPlaying(HitReactMontage) == true)
		return;
	if (Montage_IsPlaying(AttackMontage) == true)
		return;


	Montage_Play(HitReactMontage, 1.0f);
}

void UMyAnimInstance::PlayRespawnMontage()
{
	if (IsValid(RespawnMontage) == false)
		return;
	Montage_Play(RespawnMontage, 1.0f);
}

void UMyAnimInstance::PlaySkill_1Montage()
{
	if (IsValid(SkillMontage_1) == false)
		return;
	Montage_Play(SkillMontage_1, 1.0f);
}

void UMyAnimInstance::PlaySkill_2Montage()
{
	if (IsValid(SkillMontage_2) == false)
		return;
	Montage_Play(SkillMontage_2, 1.0f);
}

void UMyAnimInstance::PlaySkill_3Montage()
{
	if (IsValid(SkillMontage_3) == false)
		return;
	Montage_Play(SkillMontage_3, 1.0f);
}

void UMyAnimInstance::UpdateState()
{
	auto Pawn = TryGetPawnOwner();
	if (IsValid(Pawn) == false)
		return;

	ARPGPlayer* master = Cast<ARPGPlayer>(Pawn);
	if (IsValid(master) == true)
	{
		IsMoving = master->GetVelocity().Size();
		IsFalling = master->GetMovementComponent()->IsFalling();
		IsRunning = master->IsRunning;
		Horizontal = master->Horizontal;
		Vertical = master->Vertical;
		return;
	}

	AMonster* monster = Cast <AMonster> (Pawn);
	switch (monster->GetState())
	{
	case Protocol::CreatureState::Idle:
		IsMoving = 0;
		break;
	case Protocol::CreatureState::Move:
		IsMoving = 1;
		break;
	case Protocol::CreatureState::Attack:
		IsMoving = 0;
		break;
	}


}

void UMyAnimInstance::OnDead()
{
	IsDead = true;
	StopAllMontages(0.1f);
}

void UMyAnimInstance::JumpToSection(int32 SectionIndex, EMontageType Type)
{
	UAnimMontage* Selected = nullptr;
	switch (Type)
	{
	case EMontageType::Attack:
			Selected = AttackMontage;
			break;
	case EMontageType::ActiveSkill_1:
			Selected = SkillMontage_1;
			break;
	}

	FName Section = *FString::Printf(TEXT("Current%d"), SectionIndex);
	Montage_JumpToSection(Section, Selected);
}

void UMyAnimInstance::AnimNotify_AttackHit()
{

	//Utils::DebugLog(TEXT("AnimNotiy Attack Hit"));
	OnAttackHit.Broadcast();
}

void UMyAnimInstance::AnimNotify_SkillStart()
{
	OnSkillStart.Broadcast();
}

void UMyAnimInstance::AnimNotify_SkillEnd()
{
	OnSkillEnd.Broadcast();
}

void UMyAnimInstance::AnimNotify_AttackStart()
{
	//Attack Collision을 활성화 해준다.
	OnAttackStart.Broadcast();


}

void UMyAnimInstance::AnimNotify_AttackEnd()
{
	//Attack Collision을 비활성화 해준다.
	OnAttackEnd.Broadcast();
}

void UMyAnimInstance::SetState(Protocol::CreatureState state)
{
	if (state == Protocol::CreatureState::Dead)
		IsDead = true;


}
