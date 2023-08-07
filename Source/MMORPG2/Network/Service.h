// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameManager.h"
#include "IocpObject.h"
#include "Listener.h"
#include "NetAddress.h"
#include <functional>


enum class ServiceType : uint8
{
	Server,
	Client
};

using SessionFactory = function<SessionRef(void)>;

class MMORPG2_API Service : public TSharedFromThis<Service>
{
public:
	Service(ServiceType type, NetAddress address, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~Service();

	virtual bool		Start() abstract;
	bool				CanStart() { return _sessionFactory != nullptr; }

	virtual void		CloseService();
	void				SetSessionFactory(SessionFactory func) { _sessionFactory = func; }

	SessionRef			CreateSession();
	void				AddSession(SessionRef session);
	void				ReleaseSession(SessionRef session);
	int32				GetCurrentSessionCount() { return _sessionCount; }
	int32				GetMaxSessionCount() { return _maxSessionCount; }

public:
	ServiceType			GetServiceType() { return _type; }
	NetAddress			GetNetAddress() { return _netAddress; }
	IocpCoreRef GetIocpCore() { return _iocpCore.ToSharedRef(); }

protected:
	USE_LOCK;
	ServiceType			_type;
	NetAddress			_netAddress = {};
	IocpCoreRef			_iocpCore;

	TSet<SessionRef>		_sessions;
	int32				_sessionCount = 0;
	int32				_maxSessionCount = 0;
	SessionFactory		_sessionFactory;
};

/*-----------------
	ClientService
------------------*/

class ClientService : public Service
{
public:
	ClientService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ClientService() {}

	virtual bool	Start() override;
};


/*-----------------
	ServerService
------------------*/

class ServerService : public Service
{
public:
	ServerService(NetAddress targetAddress, IocpCoreRef core, SessionFactory factory, int32 maxSessionCount = 1);
	virtual ~ServerService() {}

	virtual bool	Start() override;
	virtual void	CloseService() override;

private:
	ListenerRef		_listener = nullptr;
};