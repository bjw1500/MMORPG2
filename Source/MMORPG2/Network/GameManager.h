// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Protocol.pb.h"
#include "../MyPlayerController.h"
#include "GameManager.generated.h"

class UObjectManager;
class NetworkManager;
class UManager;
class ClientPacketHandler;
class PacketQueue;
class UDataTableManager;
class UUIManager;
class UInventoryManager;

UCLASS()
class MMORPG2_API UGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	~UGameManager();

	virtual void Init() override;
	void LoadGameLevel(FString name);



public:
	UObjectManager* GetObjectManager();
	NetworkManager* GetNetworkManager();
	ClientPacketHandler* GetPacketHandler();
	PacketQueue* GetPacketQueue();
	UDataTableManager* GetDataManager();
	UUIManager* GetUIManager();
	UInventoryManager* GetInventory();
	AMyPlayerController* GetPlayerController() {
		return Cast<AMyPlayerController>(GetFirstLocalPlayerController());
	}

private:

	TSharedPtr<NetworkManager> _netWorkManager;
	TSharedPtr<ClientPacketHandler> _packetHandler;
	TSharedPtr<PacketQueue> _packetQueue;
	UPROPERTY()
	TObjectPtr<UObjectManager> _objectManager;
	UPROPERTY()
	TObjectPtr<UDataTableManager> _dataManager;
	UPROPERTY()
	TObjectPtr<UUIManager> UIManger;
	UPROPERTY()
	TObjectPtr<UInventoryManager> InventoryManager;

	AMyPlayerController* MyPlayerController = nullptr;

private:
	TSubclassOf<class ACreature> SpawnCharacter;

public:
	//서버 연결?
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Server")
	bool bOnline = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Server")
	bool bConnected = false;
};

//#define GameInstance  Cast<UGameManager>(GetGameInstance());

extern UGameManager* GameInstance;
class Utils
{
public:
	static void DebugLog(FString string);



};