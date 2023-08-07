// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketUtils.h"
/*----------------
	SocketUtils
-----------------*/

LPFN_CONNECTEX		SocketUtils::ConnectEx = nullptr;
LPFN_DISCONNECTEX	SocketUtils::DisconnectEx = nullptr;
LPFN_ACCEPTEX		SocketUtils::AcceptEx = nullptr;

void SocketUtils::Init()
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), OUT & wsaData);

	/* ��Ÿ�ӿ� �ּ� ������ API */
	SOCKET dummySocket = CreateSocket();
	//assert(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)));
	//assert(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
	{
		GUID guid = WSAID_CONNECTEX;
		DWORD bytes = 0;
		int rc = WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&ConnectEx, sizeof(ConnectEx),
			&bytes, NULL, NULL);
	}

	{
		GUID guid = WSAID_DISCONNECTEX;
		DWORD bytes = 0;
		int rc = WSAIoctl(dummySocket, SIO_GET_EXTENSION_FUNCTION_POINTER,
			&guid, sizeof(guid),
			&DisconnectEx, sizeof(DisconnectEx),
			&bytes, NULL, NULL);
	}


	if(ConnectEx == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("SocketUtils ConnectEX Fail"));
	if(DisconnectEx == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("SocketUtils DisConnect Fail"));

	UE_LOG(LogTemp, Warning, TEXT("SocketUtils Init"));
	Close(dummySocket);
}

void SocketUtils::Clear()
{
	::WSACleanup();
}

bool SocketUtils::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL);
}

SOCKET SocketUtils::CreateSocket()
{
	return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketUtils::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtils::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtils::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtils::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtils::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

// ListenSocket�� Ư���� ClientSocket�� �״�� ����
bool SocketUtils::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtils::Bind(SOCKET socket, NetAddress netAddr)
{
	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN));
}

bool SocketUtils::Bind(SOCKET socket, SOCKADDR_IN sockAddr)
{
	return SOCKET_ERROR != ::bind(socket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR_IN));
}

bool SocketUtils::BindAnyAddress(SOCKET socket, uint16 port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return SOCKET_ERROR != ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(myAddress));
}

bool SocketUtils::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR != ::listen(socket, backlog);
}

void SocketUtils::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}