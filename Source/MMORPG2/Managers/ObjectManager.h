// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Creature.h"
#include "Object/Monster.h"
#include "Object/MyItem.h"
#include "Object/RPGPlayer.h"
#include "Network/Protocol.pb.h"
#include "ObjectManager.generated.h"

/**
 * 
 */
UCLASS()
class MMORPG2_API UObjectManager : public UObject
{	
	GENERATED_BODY()
	friend class UGameManager;
public:
	
	void Init();
	void CreateMyPlayer(Protocol::ObjectInfo info);
	void CreateObject(Protocol::ObjectInfo info);
	AMyItem* CreateItem(Protocol::ItemInfo* info);
	void DespawnObject(Protocol::ObjectInfo* info);

	ARPGPlayer* GetMyPlayer() { return MyPlayer; }
	ACreature* GetPlayerByID(int32 id);
	AMyItem* GetItemByID(int32 id);
	void RemoveItemByID(int32 id);

	void CheckDuplicatedID(Protocol::ObjectInfo* info);


private:
	UPROPERTY();
	ARPGPlayer* MyPlayer = nullptr;
	UPROPERTY();
	TMap<int32, ARPGPlayer*> Players;
	UPROPERTY();
	TMap<int32, ACreature*> Monsters;
	UPROPERTY();
	TMap<int32, AMyItem*> Items;
	UPROPERTY();
	TSubclassOf<ACreature> SpawnCharacter;
	UPROPERTY();
	TSubclassOf<AMonster> SpawnMonster;

};
