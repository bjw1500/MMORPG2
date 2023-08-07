// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameManager.h"
#include "IocpObject.h"

/**
 * 
 */
class MMORPG2_API Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/* �ܺο��� ��� */
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	/* �������̽� ���� */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* ���� ���� */
	void RegisterAccept(IocpEvent* acceptEvent);
	void ProcessAccept(IocpEvent* acceptEvent);

protected:
	SOCKET _socket = INVALID_SOCKET;
	vector<IocpEvent*> _acceptEvents;
	ServerServiceRef _service;
};
