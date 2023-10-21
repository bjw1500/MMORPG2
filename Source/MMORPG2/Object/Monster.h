// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "GameStruct.h"
#include "Monster.generated.h"

/**
 * 
 */

UCLASS()
class MMORPG2_API AMonster : public ACreature
{
	GENERATED_BODY()
public:
	AMonster();
	
	virtual void BeginPlay();
	virtual void PostInitializeComponents();
	virtual void Tick(float DeltaSeconds) override;


	UFUNCTION(BlueprintCallable)
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION(BlueprintCallable)
	void OnWeaponOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void OnDead() override;
	void RotationToTarget();
	virtual void SyncPos() override;

	void UpdateInfo(Protocol::ObjectInfo* info) override;
	void UpdateController();
	void UpdateIdle();
	void UpdateMove();
	void UpdateAttack();
	void UpdateDead();
	void UpdateCasting();

	FMonsterData* GetMonsterData() { return &MonsterData; }
	void SetMonsterData(FMonsterData Data) { MonsterData = Data; }

private:

	bool IsTracking = false;
	FMonsterData MonsterData;
};
