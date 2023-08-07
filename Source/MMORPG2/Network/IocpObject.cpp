// Fill out your copyright notice in the Description page of Project Settings.
#include "IocpObject.h"
#include "GameManager.h"


/*--------------
	IocpCore
---------------*/

IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	assert(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore()
{
	Utils::DebugLog(TEXT("~IocpCore"));
	::CloseHandle(_iocpHandle);
}

void IocpCore::Register(IocpObjectRef iocpObject)
{
	::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&key), OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			//// TODO : �α� ���
			//IocpObjectRef iocpObject = iocpEvent->owner;
			//iocpObject->Dispatch(iocpEvent, numOfBytes);
			break;
		}
	}

	return true;
}

/*--------------
	IocpEvent
---------------*/

IocpEvent::IocpEvent(EventType type) : type(type)
{
	Init();
}

void IocpEvent::Init()
{
	hEvent = 0;
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
}
