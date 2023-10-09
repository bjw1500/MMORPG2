// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Network/Protocol.pb.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Object/GameStruct.h"
#include "SkillComponent.generated.h"


/*
스킬 컴포넌트가 해야 할 일이 뭐가 있을까?

1.캐릭터 정보를 받아서 스킬을 불러오는 역할을 수행한다.

2.스킬 사용
*/
class USkillBase;
class ACreature;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MMORPG2_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

	void SetInfo();
	void LoadSkills(Protocol::Stat* CreatureStat);
	bool ActiveSkill(SkillType Type);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TMap<int32, USkillBase*> Skills;
	UPROPERTY()
	ACreature* Master;
		
};
