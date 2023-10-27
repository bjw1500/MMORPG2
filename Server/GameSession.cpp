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
		_myPlayer->Disconnect();
	}

	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
	
}

void GameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketHeader header = *((PacketHeader*)buffer);
	ServerPacketHandler::HandlePacket(GetSessionRef(), buffer, len);
}

void GameSession::OnSend(int32 len)
{

}