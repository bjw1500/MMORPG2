// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/MyItem.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API AWeapon : public AMyItem
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AWeapon();
	UPROPERTY(EditAnyWhere)
	UCapsuleComponent* WeaponCollision;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
		virtual void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
		void OnWeaponOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
};
