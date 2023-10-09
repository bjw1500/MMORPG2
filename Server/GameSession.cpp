#include "pch.h"
#include "Player.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "GameSessionManager.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));

	cout << "새로운 플레이어 서버 연결" << endl;
	Send(ServerPacketHandler::Make_S_Connect());
}

void GameSession::OnDisconnected()
{
	if (_myPlayer != nullptr)
	{
		Protocol::ObjectInfo info = _myPlayer->GetInfo();
		_myPlayer->GetRoomRef()->Remove(_myPlayer->GetInfo());
		Send(ServerPacketHandler::Make_S_Disconnect(info));
		
		//연결이 끊어지면 데이터 베이스에 저장.
		DBConnection* db = GDBConnectionPool->Pop();
		db->SavePlayerInfo(_myPlayer);
		GDBConnectionPool->Push(db);

		printf("ID[%d], Name[%s]의 연결이 끊어졌습니다.", info.id(), info.name().c_str());
	}

	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
	
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketHeader header = *((PacketHeader*)buffer);
	//cout << "Packet ID : " << header.id << "Size : " << header.size << endl;
	
	ServerPacketHandler::HandlePacket(GetSessionRef(), buffer, len);
}

void GameSession::OnSend(int32 len)
{
	//cout << "OnSend Len = " << len << endl;
}