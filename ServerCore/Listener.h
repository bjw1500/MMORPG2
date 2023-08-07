#pragma once
#include "IocpCore.h"
#include "NetAddress.h"

class AcceptEvent;

/*--------------
	Listener
---------------*/

class Listener : public IocpObject
{
public:
	Listener() = default;
	~Listener();

public:
	/* 외부에서 사용 */
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	/* 인터페이스 구현 */
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(struct IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	/* 수신 관련 */
	void RegisterAccept(IocpEvent* acceptEvent);
	void ProcessAccept(IocpEvent* acceptEvent);

protected:
	SOCKET _socket = INVALID_SOCKET;
	vector<IocpEvent*> _acceptEvents;
	ServerServiceRef _service;
};

