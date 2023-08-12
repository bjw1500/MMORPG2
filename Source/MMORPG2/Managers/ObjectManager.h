// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Creature.h"
#include "../Monster.h"
#include "../MyItem.h"
#include "../Network/Protocol.pb.h"

/**
 * 
 */
class MMORPG2_API ObjectManager
{
	friend class UGameManager;

public:
	ObjectManager();
	~ObjectManager();

	void Init();
	void CreateMyPlayer(Protocol::ObjectInfo info);
	void CreateObject(Protocol::ObjectInfo info);
	void DespawnObject(Protocol::ObjectInfo info);

	ACreature* GetMyPlayer() { return MyPlayer; }
	ACreature* GetPlayerByID(int32 id);
	AMyItem* GetItemByID(int32 id);
	void RemoveItemByID(int32 id);

	void CheckDuplicatedID(Protocol::ObjectInfo* info);
	

private:
	ACreature* MyPlayer = nullptr;
	TMap<int32, ACreature*> Players;
	TMap<int32, ACreature*> Monsters;
	TMap<int32, AMyItem*> Items;

	TSubclassOf<ACreature> SpawnCharacter;
	TSubclassOf<AMonster> SpawnMonster;



};
