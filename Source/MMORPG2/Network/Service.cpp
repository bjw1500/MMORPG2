// Fill out your copyright notice in the Description page of Project Settings.


#include "Service.h"
#include "SocketUtils.h"
#include "NetworkSession.h"
#include "Listener.h"

/*-------------
	Service
--------------*/

Service::Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: _type(type), _netAddress(address), _iocpCore(core), _sessionFactory(factory), _maxSessionCount(maxSessionCount)
{

}

Service::~Service()
{
	Utils::DebugLog(TEXT("~Service"));
}

void Service::CloseService()
{
	// TODO
	Utils::DebugLog(TEXT("Close Service"));
	//::CloseHandle(GetIocpCore()->GetHandle())

}

SessionRef Service::CreateSession()
{
	SessionRef session = _sessionFactory();
	session->SetService(SharedThis(this));

	_iocpCore->Register(session);

	return session.ToSharedRef();
}

void Service::AddSession(SessionRef session)
{
	WRITE_LOCK;
	_sessionCount++;
	_sessions.Add(session);
}

void Service::ReleaseSession(SessionRef session)
{
	WRITE_LOCK;
	assert(_sessions.Remove(session) != 0);
	_sessionCount--;
}

/*-----------------
	ClientService
------------------*/

ClientService::ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Client, targetAddress, core, factory, maxSessionCount)
{
}

bool ClientService::Start()
{
	if (CanStart() == false)
		return false;

	SessionRef session = CreateSession();
	if (session->Connect() == false)
		return false;

	return true;
}

ServerService::ServerService(NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount)
	: Service(ServiceType::Server, address, core, factory, maxSessionCount)
{
}

bool ServerService::Start()
{
	if (CanStart() == false)
		return false;

	_listener = MakeShared<Listener>();
	if (_listener == nullptr)
		return false;

	ServerServiceRef service = StaticCastSharedRef<ServerService>(SharedThis(this));
	if (_listener->StartAccept(service) == false)
		return false;

	return true;
}

void ServerService::CloseService()
{
	// TODO

	Service::CloseService();
}