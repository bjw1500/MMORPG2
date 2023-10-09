// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameUI.generated.h"

/**
 * 인게임 UI
1.HP Bar

보스랑 일반 몹의 HP바를 따로 구분해주자.


2.채팅창
3.보스 HP 

 * 
 */

class UChatWidget;
class UPlayerUI;
class UMonsterUI;

UCLASS()
class MMORPG2_API UInGameUI : public UUserWidget
{
	GENERATED_BODY()
public:
	//UI 연결은 어떻게 해줘야 할까.



	//채팅창
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UChatWidget* ChatUI;

	//플레이어 HP Bar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UPlayerUI* PlayerUI;

	//몬스터 HP Bar
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UMonsterUI* MonsterUI;
	
	//스킬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class USkillUI* SkillUI;

	//인벤토리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UInventoryUI* InventoryUI;

	//퀘스트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UQuestUI* QuestUI;

};
