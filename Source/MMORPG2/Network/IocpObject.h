// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameManager.h"

/**
 * 
 */
class MMORPG2_API IocpObject : public TSharedFromThis<IocpObject>
{
public:
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};


/*--------------
	IocpCore
---------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE		GetHandle() { return _iocpHandle; }

	void		Register(IocpObjectRef iocpObject);
	bool		Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE		_iocpHandle;
};



class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

/*--------------
	IocpEvent
---------------*/

struct IocpEvent : public OVERLAPPED
{
	IocpEvent(EventType type);

	void		Init();

	EventType		type;
	IocpObjectRef	owner = nullptr;
	SessionRef		session = nullptr; // Accept Only

	// TEMP
	vector<BYTE> buffer;
	vector<SendBufferRef> sendBuffers;
};