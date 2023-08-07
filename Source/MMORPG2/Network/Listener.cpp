// Fill out your copyright notice in the Description page of Project Settings.


#include "Listener.h"
#include "SocketUtils.h"
#include "NetworkSession.h"
#include "Service.h"
/*--------------
	Listener
---------------*/

Listener::~Listener()
{
	SocketUtils::Close(_socket);

	for (IocpEvent* acceptEvent : _acceptEvents)
	{
		// TODO
		delete acceptEvent;
	}
}

bool Listener::StartAccept(ServerServiceRef service)
{
	_service = service;
	if (_service == nullptr)
		return false;

	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		return false;

	_service->GetIocpCore()->Register(AsShared());

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::SetLinger(_socket, 0, 0) == false)
		return false;

	if (SocketUtils::Bind(_socket, _service->GetNetAddress()) == false)
		return false;

	if (SocketUtils::Listen(_socket) == false)
		return false;

	const int32 acceptCount = _service->GetMaxSessionCount();
	for (int32 i = 0; i < acceptCount; i++)
	{
		IocpEvent* acceptEvent = new IocpEvent(EventType::Accept);
		acceptEvent->owner = AsShared();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtils::Close(_socket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Listener::Dispatch(IocpEvent* acceptEvent, int32 numOfBytes)
{
	//assert(acceptEvent->type == EventType::Accept);
	ProcessAccept(acceptEvent);
}

void Listener::RegisterAccept(IocpEvent* acceptEvent)
{
	SessionRef session = _service->CreateSession(); // Register IOCP

	acceptEvent->Init();
	acceptEvent->session = session;

	DWORD bytesReceived = 0;
	if (false == SocketUtils::AcceptEx(_socket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptEvent)))
	{
		const int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			// �ϴ� �ٽ� Accept �ɾ��ش�
			RegisterAccept(acceptEvent);
		}
	}
}

void Listener::ProcessAccept(IocpEvent* acceptEvent)
{
	SessionRef session = acceptEvent->session;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _socket))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 sizeOfSockAddr = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr))
	{
		RegisterAccept(acceptEvent);
		return;
	}

	//cout << "Client Connected!" << endl;

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();
	RegisterAccept(acceptEvent);
}