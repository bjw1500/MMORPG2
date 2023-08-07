// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyItem.generated.h"

UCLASS()
class MMORPG2_API AMyItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;


public:
	UPROPERTY(EditAnyWhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly)
	USkeletalMeshComponent* WeaponMeshComponent;
	UPROPERTY(EditAnyWhere)
	USkeletalMesh* WeaponMesh;

public:

	UFUNCTION()
	virtual void OnCharacterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
   
	

public:
	void LoadItemInfo(int32 id);


public:
	class ACreature* Master;

};
