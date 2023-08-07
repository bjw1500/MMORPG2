// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
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
	
	virtual void Tick(float DeltaSeconds) override;

	float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void RotationToTarget();
	void UpdateInfo(Protocol::ObjectInfo* info) override;

	void UpdateController();
	void UpdateIdle();
	void UpdateMove();
	void UpdateAttack();
	void UpdateDead();

	
};
