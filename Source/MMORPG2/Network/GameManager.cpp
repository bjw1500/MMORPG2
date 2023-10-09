// Fill out your copyright notice in the Description page of Project Settings.


#include "GameManager.h"
#include "SocketUtils.h"
#include "Service.h"
#include "NetworkSession.h"
#include "Managers/ObjectManager.h"
#include "Managers/NetworkManager.h"
#include "Managers/DataTableManager.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Managers/UIManager.h"
#include "Managers/InventoryManager.h"

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




UObjectManager* UGameManager::GetObjectManager()
{
	if (_objectManager.Get() == nullptr)
	{
		_objectManager = NewObject<UObjectManager>();
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
		_packetHandler->Init();
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

UDataTableManager* UGameManager::GetDataManager()
{
	if (_dataManager == false)
	{

		_dataManager = NewObject<UDataTableManager>();
		_dataManager->Init();

	}
	return _dataManager.Get();
}

UUIManager* UGameManager::GetUIManager()
{
	if (UIManger.Get() == nullptr)
		UIManger = NewObject<UUIManager>();


	return UIManger.Get();
}

UInventoryManager* UGameManager::GetInventory()
{
	if (InventoryManager.Get() == nullptr)
		InventoryManager = NewObject<UInventoryManager>();

	return InventoryManager.Get();
}

void Utils::DebugLog(FString string)
{
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, *string);
		//UE_LOG(LogTemp, Warning, TEXT("%s"), *string);
	}
}
