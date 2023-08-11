// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Network/Protocol.pb.h"
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
   
	// declare overlap end function
	UFUNCTION()
	void OnCharacterOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

public:
	void LoadItemInfo(int32 id);

	void SetItemInfo(Protocol::ItemInfo& info) { _itemInfo.CopyFrom(info); }
	Protocol::ItemInfo& GetItemInfo() { return _itemInfo; }


public:
	class ACreature* Master;


private:
	Protocol::ItemInfo _itemInfo;

};
