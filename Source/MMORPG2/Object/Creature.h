// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Network/Protocol.pb.h"
#include "StatComponent.h"
#include "Creature.generated.h"




DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitBox, ACreature*);
UCLASS(config = Game)
class ACreature : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;


public:
	ACreature();
	virtual bool Init();

public:
	//Action
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable, Category = "Server")
	virtual void Attack();
	void Dash();
	void Run();
	void RunStop();
	virtual void Jump() override;
	void TryAttack();
	void TryDash();
	void TryActiveSkill_1();
	void TryActiveSkill_2();
	void TryActiveSkill_3();
	void ActiveSkill_1();
	void ActiveSkill_2();
	void ActiveSkill_3();



protected:
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void PostInitializeComponents();

	virtual void LoadStat(int id);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY()
	class UMyAnimInstance* Anim;

	UPROPERTY()
	class UStatComponent* Stat;

	UPROPERTY()
	class USkillComponent* Skill;

	UPROPERTY()
	class UWidgetComponent* HpBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* WeaponCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* HitBoxCollision;

public:

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRollingMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnSkill_1MontageEnd(UAnimMontage* Montage, bool bInterrupted);

	//Animation Move
public:
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float Horizontal = 0.f;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	float Vertical = 0.f;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	int32 AttackIndex = 0;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	int32 StampedeIndex = 0;


public:
	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly)
	bool IsAttacking = false;
	bool CanNextCombo = false;
	bool IsRolling = false;
	bool IsRunning = false;
	bool IsCasting = false;
	bool IsNoDamage = false;

	//네트워크
public:
	Protocol::ObjectInfo* GetInfo() { return &Info; }
	void									Setinfo(Protocol::ObjectInfo info) {
		Info.CopyFrom(info); Stat->SetInfo();
		UpdateInfo(&Info);
	}
	Protocol::CreatureState GetState() { return Info.state(); }

	void UseSkill(Protocol::Skill_ID id);
	virtual void AttackCheck();
	virtual void AttackStart();
	virtual void AttackEnd();

	virtual  float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDead();
	virtual void	UpdateInfo(Protocol::ObjectInfo* info);
	virtual void SyncPos();
	void UpdatePlayerSkill(int32 skillId, int32 attackIndex);
	UFUNCTION()
	void OnHitBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FOnHitBox OnHitBox;
	bool ThisMasterOtherClient = false;
	bool IsSync = false;
protected:
	
	Protocol::ObjectInfo Info;


};


