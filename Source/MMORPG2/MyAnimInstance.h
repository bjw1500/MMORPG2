// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Network/Protocol.pb.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */


DECLARE_MULTICAST_DELEGATE(FOnAttackHit);
DECLARE_MULTICAST_DELEGATE(FOnAttackStart);
DECLARE_MULTICAST_DELEGATE(FOnAttackEnd);

UENUM()
enum EMontageType
{
	Attack,
	ActiveSkill_1,
};

UCLASS()
class MMORPG2_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UMyAnimInstance();
		virtual void NativeUpdateAnimation (float DeltaSeconds) override;
		void PlayAttackMontage();
		void PlayDashMontage();
		void PlayHitReactMontage();
		void PlayRespawnMontage();
		void PlaySkill_1Montage();
		void PlaySkill_2Montage();
		void PlaySkill_3Montage();
		void UpdateState();
		void OnDead();

		void JumpToSection(int32 SectionIndex, EMontageType Type);

		UFUNCTION()
		void AnimNotify_AttackHit();

		UFUNCTION()
		void AnimNotify_SkillStart();

		UFUNCTION()
		void AnimNotify_SkillEnd();

		UFUNCTION()
		void AnimNotify_AttackStart();

		UFUNCTION()
		void AnimNotify_AttackEnd();

		void SetState(Protocol::CreatureState state);


private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	float Horizontal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	float Vertical;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	float IsMoving;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsRolling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsRunning = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsEquipped = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* SkillMontage_1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* SkillMontage_2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
		UAnimMontage* SkillMontage_3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* DashMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* RespawnMontage;

public:
	FOnAttackHit OnSkillEnd;
	FOnAttackHit OnSkillStart;
	FOnAttackHit OnAttackHit;
	FOnAttackStart OnAttackStart;
	FOnAttackEnd OnAttackEnd;

};
