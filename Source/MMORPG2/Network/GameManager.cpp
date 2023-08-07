// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "SocketUtils.h"
#include "Service.h"
#include "NetworkSession.h"
#include "../Managers//Manager.h"
#include "../Managers/ObjectManager.h"
#include "../Managers/NetworkManager.h"
#include "../Managers/DataManager.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UGameManager* GameInstance = nullptr;

UGameManager::~UGameManager()
{
	Utils::DebugLog(TEXT("~UGameManager"));
	ThreadManager::GetInstance()->Join();
	GetPacketQueue()->PopAll();

	//연결된 네트워크 끊어주기;
	//GetNetworkManager()->Disconnect();


}

void UGameManager::Init()
{
	Super::Init();
	GameInstance = this;
	Utils::DebugLog(TEXT("UGameManager Init"));

}

void UGameManager::LoadGameLevel(FString name)
{
	

	Utils::DebugLog(name);
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), *name, true);

}




ObjectManager* UGameManager::GetObjectManager()
{
	if (_objectManager.IsValid() == false)
	{
		_objectManager = MakeShared<ObjectManager>();
		_objectManager->Init();
	}

	return _objectManager.Get();
}

NetworkManager* UGameManager::GetNetworkManager()
{
	if (_netWorkManager.IsValid() == false)
	{
		_netWorkManager = MakeShared<NetworkManager>();
	}


	return _netWorkManager.Get();
}

ClientPacketHandler* UGameManager::GetPacketHandler()
{
	if (_netWorkManager.IsValid() == false)
	{
		_packetHandler = MakeShared<ClientPacketHandler>();
	}


	return _packetHandler.Get();
}

PacketQueue* UGameManager::GetPacketQueue()
{
	if (_packetQueue.IsValid() == false)
	{

		_packetQueue = MakeShared<PacketQueue>();

	}
	return _packetQueue.Get();
}

DataManager* UGameManager::GetDataManager()
{
	if (_dataManager.IsValid() == false)
	{

		_dataManager = MakeShared<DataManager>();
		_dataManager->Init();

	}
	return _dataManager.Get();
}

void Utils::DebugLog(FString string)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *string);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
	}
}
