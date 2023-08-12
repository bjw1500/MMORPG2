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

UCLASS()
class MMORPG2_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UMyAnimInstance();
		virtual void NativeUpdateAnimation (float DeltaSeconds) override;
		void PlayAttackMontage();
		void PlayRollingMontage();

		UFUNCTION()
		void AnimNotify_AttackHit();

		void SetState(Protocol::CreatureState state);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	float IsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsFalling;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsDead = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsRolling = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	bool IsEquipped = false;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* AttackMontage;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Move, Meta = (AllowPrivateAccess = True))
	UAnimMontage* RollingMontage;

public:
	FOnAttackHit OnAttackHit;

};
