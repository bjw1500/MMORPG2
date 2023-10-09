// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/GameStruct.h"
#include "SkillBase.generated.h"

/**
 * 
 */

class ACreature;
UCLASS()
class MMORPG2_API USkillBase : public UObject
{
	GENERATED_BODY()

public:
	USkillBase();

	virtual void Active();
	virtual void ActiveEnd();

	virtual void Update();

	void SetSkillData(FSkillData Data) { SkillData = Data; }
	FSkillData& GetSkillData() { return SkillData; }
	void SetMaster(ACreature* master) { Master = master; };
	ACreature* GetMaster() { return Master.Get(); }


protected:
	UPROPERTY()
	FSkillData SkillData;
	UPROPERTY()
	TWeakObjectPtr<ACreature> Master;
	FTimerHandle SkillTimer;

};


UCLASS()
class MMORPG2_API USkillActive : public USkillBase
{
	GENERATED_BODY()
public:
	USkillActive();

	virtual void Active() override;
	virtual void ActiveEnd();
	virtual void Update() override;
};




///////////////Skill List
UCLASS()
class MMORPG2_API USkill_Stampede : public USkillActive
{
	GENERATED_BODY()
public:
	USkill_Stampede();

	virtual void Active() override;
	virtual void ActiveEnd();
	virtual void Update() override;
	UFUNCTION()
	void Stampede(ACreature* target);

private:
	float SkillSpeed;
	float NormalSpeed;
};

UCLASS()
class MMORPG2_API USkill_ShockWave: public USkillActive
{
	GENERATED_BODY()
public:
	USkill_ShockWave();

	virtual void Active() override;
	virtual void ActiveEnd();
	virtual void Update() override;
	UFUNCTION()
	void ShockWave(ACreature* target);

private:

	FVector TargetLocation;
};

//Grux 체력 절반 떨어지면 사용되는 스킬
UCLASS()
class MMORPG2_API USkill_Roar : public USkillActive
{
	GENERATED_BODY()
public:
	USkill_Roar();

	virtual void Active() override;
	virtual void ActiveEnd();
	UFUNCTION()
	void Roar();
};


UCLASS()
class MMORPG2_API UFlashAttack : public USkillActive
{
	GENERATED_BODY()
public:
	UFlashAttack();

	virtual void Active() override;
	UFUNCTION()
	virtual void ActiveEnd();

private:
	FRotator Rotation;
	FVector Destination;
};


//Shinbi TripleBash
UCLASS()
class MMORPG2_API UDash : public USkillActive
{
	GENERATED_BODY()
public:
	UDash();

	virtual void Active() override;
	UFUNCTION()
	virtual void ActiveEnd();

private:
	float SkillSpeed;
	float NormalSpeed;
};

UCLASS()
class MMORPG2_API UTripleBash : public USkillActive
{
	GENERATED_BODY()
public:
	UTripleBash();

	virtual void Active() override;
	UFUNCTION()
		virtual void ActiveEnd();

private:
	FVector Dest;

};


//Shinbi PhantomSlash
UCLASS()
class MMORPG2_API UPhantomSlash : public USkillActive
{
	GENERATED_BODY()
public:
	UPhantomSlash();

	virtual void Active() override;
	UFUNCTION()
		virtual void ActiveEnd();

private:
	FVector Dest;

};