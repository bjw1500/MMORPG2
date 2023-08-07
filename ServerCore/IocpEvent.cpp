#include "pch.h"
#include "IocpEvent.h"

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
