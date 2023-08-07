// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Protocol.pb.h"
#include "../MyPlayerController.h"
#include "GameManager.generated.h"

class ObjectManager;
class NetworkManager;
class UManager;
class ClientPacketHandler;
class PacketQueue;
class DataManager;

UCLASS()
class MMORPG2_API UGameManager : public UGameInstance
{
	GENERATED_BODY()

public:
	~UGameManager();

	virtual void Init() override;
	void LoadGameLevel(FString name);



public:
	ObjectManager* GetObjectManager();
	NetworkManager* GetNetworkManager();
	ClientPacketHandler* GetPacketHandler();
	PacketQueue* GetPacketQueue();
	DataManager* GetDataManager();
	AMyPlayerController* GetPlayerController() {
		return Cast<AMyPlayerController>(GetFirstLocalPlayerController());
	}

private:
	TSharedPtr<ObjectManager> _objectManager;
	TSharedPtr<NetworkManager> _netWorkManager;
	TSharedPtr<ClientPacketHandler> _packetHandler;
	TSharedPtr<PacketQueue> _packetQueue;
	TSharedPtr<DataManager> _dataManager;

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