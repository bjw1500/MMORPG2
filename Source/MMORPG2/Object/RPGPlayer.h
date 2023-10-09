// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "RPGPlayer.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API ARPGPlayer : public ACreature
{
	GENERATED_BODY()
public:
	ARPGPlayer();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();
	virtual void PostInitializeComponents();
	
	virtual void OnDead() override;
	virtual void Destroyed() override;



	/*
	Input 설정
	코드로 변수는 만들어주되 BP에서 직접 설정해주자.
	*/
public:
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ActiveSkillAction1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ActiveSkillAction2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ActiveSkillAction3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShowInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenQuestAction;



	//액션
public:
	void FindNearItem(class AMyItem* Item);
	void LoseNearItem(AMyItem* Item);
	void PickUpItem();
	void AddToInventory(Protocol::ItemInfo* ItemInfo);
	void EquippedItem(Protocol::ItemInfo* Item);
	void CreateItem();
	void ShowChat();
	void ShowInventory();
	void InteractNPC();
	void OpenQuest();


	virtual  float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
public:
	FName WeaponSocket = (TEXT("WeaponSocket"));
	UPROPERTY()
	TWeakObjectPtr<class AWeapon> CurrentUseWeapon;
	UPROPERTY()
	TArray<class AMyItem*> CanPickUpItemList;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TSubclassOf<class UCameraShakeBase> ShakeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	TSubclassOf<class UUserWidget> HitFlash;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	class USoundCue* SoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle")
	class UParticleSystem* AttackImpact;

	UPROPERTY()
	class ANPC* CanInteractNPC;

};
