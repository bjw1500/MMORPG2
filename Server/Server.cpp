#include "pch.h"
#include <iostream>
#include <thread>
#include <vector>
using namespace std;
#include <atomic>
#include <mutex>
#include "ThreadManager.h"
#include "SocketUtils.h"
#include "Listener.h"
#include "Service.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ServerPacketHandler.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"
#include "DataManager.h"
#include "GameRoom.h"




int main()
{

	GDBConnectionPool = new DBConnectionPool();
	//DB 테스트
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={SQL Server Native Client 11.0};Server=(localdb)\\MSSQLLocalDB;Database=Unreal_Server;Trusted_Connection=Yes;"));

	GDataManager = new DataManager();
	//GDataManager->MakeJsonTest();


	SocketUtils::Init();

	ServerServiceRef service = make_shared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		make_shared<IocpCore>(),
		[](){ return make_shared<GameSession>(); }, // TODO : SessionManager 등
		10);

	assert(service->Start());


	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	GRoom->EnterMonster();

	//게임 Update
	while (true)
	{
		GRoom->Update(0.05);
		this_thread::sleep_for(50ms);
	}


	GThreadManager->Join();
	
	// 윈속 종료
	SocketUtils::Clear();	
}