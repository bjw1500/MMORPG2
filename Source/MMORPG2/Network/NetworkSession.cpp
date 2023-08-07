// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkSession.h"
#include "SocketUtils.h"
#include "Service.h"
#include "../Managers/NetworkManager.h"
#include "GameManager.h"
#include "ClientPacketHandler.h"



/*--------------
	Session
---------------*/

Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
}

Session::~Session()
{
	Utils::DebugLog(TEXT("~Session"));
	SocketUtils::Close(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	bool registerSend = false;

	{
		WRITE_LOCK;
		_sendQueue.push(sendBuffer);
		registerSend = _sendRegistered.exchange(true) == false;
	}

	/*if (_sendRegistered == false)
	{
		_sendRegistered = true;
		RegisterSend();
	}*/

	if (registerSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;

	// TEMP
	wcout << "Disconnect : " << cause << endl;

	OnDisconnected(); // ������ �ڵ忡�� ������
	//GetService()->ReleaseSession(GetSessionRef());

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->type)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0/*���°�*/) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = SharedThis(this); // ADD_REF

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetNetAddress().GetSockAddr();

	if (false == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = SharedThis(this); // ADD_REF

	if (false == SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = SharedThis(this); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &_recvEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; // RELEASE_REF
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = SharedThis(this); // ADD_REF

	// ���� �����͸� sendEvent�� ���?
	{
		WRITE_LOCK;

		int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			SendBufferRef sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : ���� üũ

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	// Scatter-Gather (�����?�ִ� �����͵��� ��Ƽ�?�� �濡 ������)
	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (SendBufferRef sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr; // RELEASE_REF
			_sendEvent.sendBuffers.clear(); // RELEASE_REF
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr; // RELEASE_REF

	_connected.store(true);

	// ���� ���?
	GetService()->AddSession(GetSessionRef());

	// ������ �ڵ忡�� ������
	OnConnected();
	UE_LOG(LogTemp, Warning, TEXT("Process Connect"));

	// ���� ���?
	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr; // RELEASE_REF
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent.owner = nullptr; // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32 dataSize = _recvBuffer.DataSize();
	int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); // ������ �ڵ忡�� ������
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	// Ŀ�� ����
	_recvBuffer.Clean();

	// ���� ���?
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent.owner = nullptr; // RELEASE_REF
	_sendEvent.sendBuffers.clear(); // RELEASE_REF

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	// ������ �ڵ忡�� ������
	OnSend(numOfBytes);

	/*WRITE_LOCK;
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();*/

	bool registerSend = false;

	{
		WRITE_LOCK;
		if (_sendQueue.empty())
			_sendRegistered.store(false);
		else
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

void Session::HandleError(int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		// TODO : Log
		//cout << "Handle Error : " << errorCode << endl;
		break;
	}
}


/*-----------------
	PacketSession
------------------*/

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

// [size(2)][id(2)][data....][size(2)][id(2)][data....]
int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;

	while (true)
	{
		int32 dataSize = len - processLen;
		// �ּ��� �����?�Ľ��� �� �־��?�Ѵ�
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		// �����?��ϵ�?��Ŷ ũ�⸦ �Ľ��� �� �־��?�Ѵ�
		if (dataSize < header.size)
			break;
		// ��Ŷ ���� ����
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}


/*
ServerSession
*/


void ServerSession::OnConnected()
{

	//FString string = TEXT("���� ����");
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("On Connected")));
	GameInstance->bConnected = true;
	//UE_Utils::ScreenDebugMessage(TEXT("���� ����!"));
}

void ServerSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	GameInstance->GetPacketHandler()->OnRecvPacket(GetServerSessionRef(), buffer, len);
}

void ServerSession::OnSend(int32 len)
{
}

void ServerSession::OnDisconnected()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, TEXT("On Disconnected!"));
	ThreadManager::GetInstance()->Join();
	GameInstance->GetPacketQueue()->PopAll();

}

