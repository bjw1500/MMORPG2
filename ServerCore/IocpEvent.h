#pragma once

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