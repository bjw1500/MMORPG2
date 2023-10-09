// Fill out your copyright notice in the Description page of Project Settings.


#include "Managers/ObjectManager.h"
#include "ObjectManager.h"
#include "Kismet/GameplayStatics.h"
#include "Network/GameManager.h"
#include "Object/Creature.h"
#include "MyPlayerController.h"
#include "Network/Protocol.pb.h"
#include "DataTableManager.h"
#include "Object/GameStruct.h"
#include "Object/Monster.h"
#include "Object/MyItem.h"

void UObjectManager::Init()
{

	UE_LOG(LogTemp, Warning, TEXT("ObjectManager Init!"));

	//오브젝트 불러오기
	SpawnCharacter =
		StaticLoadClass(ACreature::StaticClass(), NULL, TEXT("/Game/Blueprints/Player/BP_Player.BP_Player_C"));
	if (IsValid(SpawnCharacter) == false)
	{
		Utils::DebugLog(TEXT("Spawn Character Load Fail"));
	}
	SpawnMonster =
		StaticLoadClass(AMonster::StaticClass(), NULL, TEXT("/Game/Blueprints/Monster/BP_Monster.BP_Monster_C"));
	if (IsValid(SpawnMonster) == false)
	{
		Utils::DebugLog(TEXT("Spawn SpawnMonster Load Fail"));
	}

}

void UObjectManager::CreateMyPlayer(Protocol::ObjectInfo info)
{
	//플레이어를 생성한다.
	//스폰 기능은 UWorld 안에 내장되어 있다.

	UWorld* const world = GameInstance->GetWorld();
	if (IsValid(world) == false)
	{
		Utils::DebugLog(TEXT("Create My Player Get World Fail"));
		return;
	}
	FVector SpawnLocation;
	SpawnLocation.X = info.position().locationx();
	SpawnLocation.Y = info.position().locationy();
	SpawnLocation.Z = info.position().locationz() + 10;

	//회전값
	FRotator SpawnRotation;
	SpawnRotation.Yaw = 0;
	SpawnRotation.Pitch = 0;
	SpawnRotation.Roll = 0;

	////스폰 파라미터
	FActorSpawnParameters SpawnParams;
	FString name = *FString(FString::FromInt(info.id()));
	//SpawnParams.Name = FName(*name);

	FCharacterData* data = GameInstance->GetDataManager()->GetCharacterData(info.templateid());
	if (data == nullptr)
	{
		Utils::DebugLog(TEXT("ObjectManager Eerror 몬스터 데이터가 없습니다."));
		return;
	}

	MyPlayer = world->SpawnActor<ARPGPlayer>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
	MyPlayer->Setinfo(info);
	MyPlayer->ThisMasterOtherClient = false;
	if (IsValid(MyPlayer) == false)
	{
		Utils::DebugLog(TEXT("Spawn Failed My Player"));
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Spawn Failed My Player")));
	}

	//FString name2 = *FString(info.name().c_str());
	//Utils::DebugLog(FString::Printf(TEXT("ID[%s] new info name"), *name2));
	Utils::DebugLog(TEXT("My Player 생성 완료"));
	AMyPlayerController* pc = GameInstance->GetPlayerController();
	if (IsValid(pc) == false)
	{
		Utils::DebugLog(TEXT("Cast faill AMyPlayerController"));
		return;
	}
	Utils::DebugLog(TEXT("Possess MyPlayer"));
	pc->bCanControll = true;
	pc->Possess(MyPlayer);
	Players.Add(MyPlayer->GetInfo()->id(), MyPlayer);
}


void UObjectManager::CreateObject(Protocol::ObjectInfo info)
{
	UWorld* const world = GameInstance->GetWorld();
	if (IsValid(world) == false)
	{
		Utils::DebugLog(TEXT("Create Object Get World Fail"));
		return;
	}

	FVector SpawnLocation;
	SpawnLocation.X = info.position().locationx();
	SpawnLocation.Y = info.position().locationy();
	SpawnLocation.Z = info.position().locationz();

	//회전값
	FRotator SpawnRotation;
	SpawnRotation.Yaw = info.position().rotationz();
	SpawnRotation.Pitch = 0;
	SpawnRotation.Roll = info.position().rotationx();

	////스폰 파라미터
	FActorSpawnParameters SpawnParams;
	FString name = *FString(FString::FromInt(info.id()));
	//SpawnParams.Name = FName(*name);



	//서버에서 MyPlayer Spawn할 때 패킷이 중복되서 날아오므로 만든 방지 코드
	if (MyPlayer->GetInfo()->id() == info.id())
	{
		Utils::DebugLog(TEXT("중복된 ID입니다."));
		return;
	}

	Protocol::ObjectType type = info.type();
	switch (type)
	{
	case Protocol::PLAYER:
	{
		FCharacterData* data = GameInstance->GetDataManager()->GetCharacterData(info.templateid());
		if (data == nullptr)
		{
			Utils::DebugLog(TEXT("ObjectManager Eerror 몬스터 데이터가 없습니다."));
			return;
		}

		ARPGPlayer* newPlayer = world->SpawnActor<ARPGPlayer>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
		if (IsValid(newPlayer) == false)
			return;
		newPlayer->Setinfo(info);
		newPlayer->ThisMasterOtherClient = true;
		Players.Add(info.id(), newPlayer);
	}
	break;
	case Protocol::MONSTER:
	{
		FMonsterData* data = GameInstance->GetDataManager()->GetMonsterData(info.templateid());
		if (data == nullptr)
		{
			Utils::DebugLog(TEXT("ObjectManager Eerror 몬스터 데이터가 없습니다."));
			return;
		}

		AMonster* newMonster = world->SpawnActor<AMonster>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
		if (IsValid(newMonster) == false)
			return;
		newMonster->Setinfo(info);
		Monsters.Add(info.id(), newMonster);
		newMonster->SyncPos();
		newMonster->ThisMasterOtherClient = true;
	}
	break;
	case Protocol::ITEM:
	{
		FItemData* data = GameInstance->GetDataManager()->GetItemData(info.templateid());
		AMyItem* newItem = world->SpawnActor<AMyItem>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
		if (IsValid(newItem) == false)
			return;
		newItem->SetItemData(data);
		//newItem->SetItemInfo(info.iteminfo());
		newItem->SetObjectInfo(info);
		Items.Add(info.id(), newItem);
		break;
	}
	case Protocol::UNKNOWN:
		break;
	default:
		break;
	}
}

AMyItem* UObjectManager::CreateItem(Protocol::ItemInfo* info)
{
	UWorld* const world = GameInstance->GetWorld();

	FVector SpawnLocation;
	SpawnLocation.X = 0;
	SpawnLocation.Y = 0;
	SpawnLocation.Z = 0;

	//회전값
	FRotator SpawnRotation;
	SpawnRotation.Yaw = 0;
	SpawnRotation.Pitch = 0;
	SpawnRotation.Roll = 0;

	////스폰 파라미터
	FActorSpawnParameters SpawnParams;
	//SpawnParams.Name = FName(*name);

	FItemData* data = GameInstance->GetDataManager()->GetItemData(info->templateid());
	AMyItem* newItem = world->SpawnActor<AMyItem>(data->Model, SpawnLocation, SpawnRotation, SpawnParams);
	if (IsValid(newItem) == false)
		return nullptr;
	newItem->SetItemData(data);
	newItem->SetItemInfo(*info);
	return newItem;
}

void UObjectManager::DespawnObject(Protocol::ObjectInfo* info)
{
	Protocol::ObjectType type = info->type();
	switch (type)
	{
	case Protocol::PLAYER:
	{
		if (Players.Contains(info->id() == false))
			return;

		ARPGPlayer* despawnTarget;
		Players.RemoveAndCopyValue(info->id(), despawnTarget);
		if (despawnTarget == nullptr)
			return;
		if (despawnTarget == MyPlayer)
		{
			MyPlayer = nullptr;
		}
		despawnTarget->SetLifeSpan(1);
	}
	break;
	case Protocol::MONSTER:
	{
		if (Monsters.Contains(info->id() == false))
			return;

		ACreature* despawnTarget;
		Monsters.RemoveAndCopyValue(info->id(), despawnTarget);
		if (IsValid(despawnTarget) == false)
			return;
		despawnTarget->SetLifeSpan(1);
	}
	break;
	case Protocol::ITEM:
		if (Items.Contains(info->id() == false))
			return;

		AMyItem* despawnTarget;
		Items.RemoveAndCopyValue(info->id(), despawnTarget);
		if (IsValid(despawnTarget) == false)
			return;
		despawnTarget->SetLifeSpan(1);
		break;
	case Protocol::UNKNOWN:
		break;
	}
}

ACreature* UObjectManager::GetPlayerByID(int32 id)
{
	bool check = Players.Contains(id);
	if (check == false)
	{
		check = Monsters.Contains(id);
		if (check == false)
			return nullptr;
		return Monsters[id];
	}

	return Players[id];
}

AMyItem* UObjectManager::GetItemByID(int32 id)
{
	bool check = Items.Contains(id);
	if (check == false)
		return nullptr;

	return Items[id];
}

void UObjectManager::RemoveItemByID(int32 id)
{
	bool check = Items.Contains(id);
	if (check == false)
		return;
	AMyItem* item;
	Items.RemoveAndCopyValue(id, item);
	GameInstance->GetWorld()->DestroyActor(item);

}


void UObjectManager::CheckDuplicatedID(Protocol::ObjectInfo* info)
{
	//혹시라도 서버에서 중복된 아이디 스폰 값들이 날아오면?
	//TODO 나중에 필요해질거 같으면 만들어주자

}