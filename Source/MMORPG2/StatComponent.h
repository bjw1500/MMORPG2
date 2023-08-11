// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Network/Protocol.pb.h"
#include "StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHpChanged);



//StatComponent가 필요할까?
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMORPG2_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	void SetInfo();
	void OnDamaged(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser);


	Protocol::Stat GetStat() { return MasterInfo->stat(); }
	float GetLevel() { return GetStat().level(); }
	float GetMaxHp() { return  GetStat().maxhp(); }
	float GetHp() { return  GetStat().hp(); }
	void SetHp(int32 value);

	float GetHpRatio() { return  GetStat().hp() / (float)GetStat().maxhp(); }
	float GetDamage() { return  GetStat().damage(); }

	FOnHpChanged OnHpChanged;
private:

	class ACreature* Master;
	Protocol::ObjectInfo* MasterInfo;
};
