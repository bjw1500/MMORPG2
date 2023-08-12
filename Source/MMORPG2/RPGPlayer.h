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


	void OnDead() override;

};
