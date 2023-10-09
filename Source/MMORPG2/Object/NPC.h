// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
#include "Object/GameStruct.h"
#include "NPC.generated.h"

UCLASS()
class MMORPG2_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void OnInteractSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnInteractSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void InteractPlayer();
public:

	UPROPERTY(EditAnyWhere)
	USphereComponent* InteractCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	TSubclassOf<class UShopItemList> ShopItemListClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	TSubclassOf<class UNPCUI> NPCUIClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Server")
	FQuestData QuestData;
};
