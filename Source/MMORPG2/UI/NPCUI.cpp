// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NPCUI.h"
#include "Object/NPC.h"
#include "Network/GameManager.h"
#include "Managers/UIManager.h"
#include "UI/ShopItemList.h"
#include "Network/ClientPacketHandler.h"

void UNPCUI::OnClickedShopButton()
{
	if (IsValid(Master) == false)
		return;

	//이미 창이 열려 있다면 닫아주기
	if (IsValid(GameInstance->GetUIManager()->PopUI) == true)
	{
		GameInstance->GetUIManager()->PopUI->RemoveFromViewport();
		GameInstance->GetUIManager()->PopUI = nullptr;
		//return;
	}
	//TODO 나중에는 상인들마다 각자 다른 ShopList를 건네주게 해주기
	if (IsValid(Master->ShopItemListClass) == false)
		return;

	UShopItemList* shopList = CreateWidget<UShopItemList>(GetWorld(), Master->ShopItemListClass);
	GameInstance->GetUIManager()->PopUI = shopList;
	shopList->RefreshUI();
	shopList->AddToViewport();

}

void UNPCUI::OnClickedBossSpawnButton()
{
	GameInstance->GetPacketHandler()->Make_C_SpawnBoss();

}

void UNPCUI::OnClickedQuestButton()
{
	//퀘스트 수락 버튼.
	GameInstance->GetPacketHandler()->Make_C_GetQuest(Master->QuestData.Id);

}
