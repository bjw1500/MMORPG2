// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Network/Buffer.h"
#include "../Network/Protocol.pb.h"
#include "../Network/NetworkSession.h"

/**
 * 
 */
class MMORPG2_API NetworkManager
{
	friend class UGameManager;

public:
	NetworkManager();
	~NetworkManager();

	bool Init(wstring ip = L"127.0.0.1");
	ServerSessionRef GetSession() { return _session; };

	ServerSessionRef CreateSession();
	void SendPacket(SendBufferRef buffer);
	void Disconnect();
	//static NetworkManager* GetNetworkManager();


private:
	ClientServiceRef _service;
	ServerSessionRef _session;
	bool IsInit = false;
	//static TSharedPtr<NetworkManager> _networkManager;
};

class ThreadManager
{
private:
	ThreadManager();
	~ThreadManager();
public:
	static ThreadManager* GetInstance();
	void	Launch(function<void(void)> callback);
	void	Join();

private:

	static void InitTLS();
	static void DestroyTLS();

private:
	mutex			_lock;
	TArray<class FWorker*>	_threads;

};


class PacketMessage
{
public:
	TArray<BYTE> pkt;

};

class PacketQueue
{
public:
	PacketQueue() {}
	~PacketQueue() {}


	//static PacketQueue* GetInstance();

	void Push(PacketMessage message) {
		//lock_guard<mutex> lock (_lock);
		_packetQueue.push(message);

	}

	TArray<PacketMessage> PopAll()
	{
		TArray<PacketMessage> list;
		{
			lock_guard<mutex> lock(_lock);
			while (_packetQueue.empty() == false)
			{

				list.Add(_packetQueue.front());
				_packetQueue.pop();
			}
		}

		return list;
	}


private:
	queue<PacketMessage> _packetQueue;
	mutex _lock;

};



