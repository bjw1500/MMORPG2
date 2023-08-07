// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkManager.h"
#include "../Network/NetworkSession.h"
#include "Kismet/GameplayStatics.h"
#include "../Network/Service.h"
#include "../Network/SocketUtils.h"
#include "../Network/FWorker.h"

NetworkManager::NetworkManager()
{
	

}

NetworkManager::~NetworkManager()
{
}

bool NetworkManager::Init(wstring ip)
{
	if (GameInstance->bConnected == true)
		return false;
	if (GameInstance->bOnline == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Online is false")));
		return false;
	}


	SocketUtils::Init();
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("Network Manager Init")));
	UE_LOG(LogTemp, Warning, TEXT("Network Manager Init"));

	_service = MakeShared<ClientService>(
		NetAddress(ip, 7777),
		MakeShared<IocpCore>(),
		[=]() { return CreateSession(); }, // TODO : SessionManager ��
		1);

	if (_service->Start() == false)
		return false;

	ThreadManager::GetInstance()->Launch(
		[=]() {

			if (_service != nullptr)
				_service->GetIocpCore()->Dispatch();

		}
	);

	return true;
}

ServerSessionRef NetworkManager::CreateSession()
{
	_session = MakeShared<ServerSession>();


	return _session;
}

void NetworkManager::SendPacket(SendBufferRef buffer)
{
	_session->Send(buffer);
}

void NetworkManager::Disconnect()
{
	if(_service != nullptr)
		_service->CloseService();

	//쓰레드부터 죽이자.

}

/********************

ThreadManager

*********************/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	lock_guard<mutex> guard(_lock);
	FWorker* worker = new FWorker(callback);
	_threads.Add(worker);
}

void ThreadManager::Join()
{
	for (FWorker* t : _threads)
	{
		t->Stop();

	}
	_threads.Empty();
}

ThreadManager* ThreadManager::GetInstance()
{
	static ThreadManager _threadManager;
	return &_threadManager;
}

void ThreadManager::InitTLS()
{

}

void ThreadManager::DestroyTLS()
{

}
