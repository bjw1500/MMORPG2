// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Network/Protocol.pb.h"
#include "StatComponent.h"
#include "Creature.generated.h"





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


	/*
	Input 설정
	코드로 변수는 만들어주되 BP에서 직접 설정해주자.
	*/

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* PickUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ShowChatAction;

public:
	ACreature();
	virtual bool Init();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Attack();
	void Rolling();

	virtual void Jump() override;

	void ShowChat();

protected:
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

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
	class UWidgetComponent* HpBar;

public:

	UFUNCTION()
	void OnAttackMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRollingMontageEnd(UAnimMontage* Montage, bool bInterrupted);

private:

	bool IsAttacking = false;
	bool IsRolling = false;

	//네트워크
public:
	Protocol::ObjectInfo* GetInfo() { return &Info; }
	void									Setinfo(Protocol::ObjectInfo info) {
		Info.CopyFrom(info);
		Stat->SetInfo();
	}

	void UseSkill(Protocol::Skill_ID id);
	void AttackCheck();
	virtual  float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnDead();

	void FindNearItem(class AMyItem* item);
	void LoseNearItem(class AMyItem* item);

	void PickUpItem();
	void EquippedItem(AMyItem* item);
	void CreateItem();

	virtual void	UpdateInfo(Protocol::ObjectInfo* info);
	void UpdatePlayerSkill(int32 skillId);


	bool ThisMasterOtherClient = false;
protected:
	
	Protocol::ObjectInfo Info;
	FName WeaponSocket = (TEXT("WeaponSocket"));
	TWeakObjectPtr<class AMyItem> CurrentUseItem;
	TArray<class AMyItem*> CanPickUpItemList;
};


