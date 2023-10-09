#include "pch.h"
#include "GameObject.h"
#include "GameRoom.h"


GameObject::GameObject()
{

}

GameObject::~GameObject()
{

}

void GameObject::Update(float deltaTime)
{
}

void GameObject::UpdateInfo()
{
	if (GetRoomRef() == nullptr)
		return;

	Protocol::S_UpdateInfo pkt;
	pkt.mutable_info()->CopyFrom(GetInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_UPDATE_INFO);
	GetRoomRef()->BroadCast(sendBuffer);
}

float GameObject::GetDistanceFromTarget(Protocol::Position pos)
{
	Protocol::Position currentPos = GetPos();
	//상대방과의 거리를 구한다.
	int32 x = currentPos.locationx() - pos.locationx();
	int32 y = currentPos.locationy() - pos.locationy();
	int32 z = currentPos.locationz() - pos.locationz();

	return sqrt(x * x + y * y + z * z);
}
