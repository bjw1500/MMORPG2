#include "pch.h"
#include "Creature.h"
#include "GameRoom.h"
Creature::Creature()
{

}

Creature::~Creature()
{
}

void Creature::OnDamaged(Protocol::ObjectInfo damageCauser, int32 damageAmount)
{
	int32 currentHp =  GetInfo().stat().hp();
	int32 result = currentHp - damageAmount;
	if (result <= 0)
	{
		result = 0;
		OnDead(damageCauser);
	}
	SetHP(result);	
	printf("%s가 %s의 공격을  받았습니다. 데미지 [%d], 남은 HP[%d]\n", 
	GetInfo().name().c_str(), damageCauser.name().c_str(), damageAmount, GetHP());
}

void Creature::OnDead(Protocol::ObjectInfo damageCauser)
{
	if (GetState() == CreatureState::Dead)
		return;

	printf("%s가 %s의 공격을 받아 사망했습니다.\n", GetInfo().name().c_str(), damageCauser.name().c_str());
	SetState(CreatureState::Dead);

	//플레이어들한테 죽음 통지
	Protocol::S_Die pkt;
	pkt.mutable_target()->CopyFrom(GetInfo());
	pkt.mutable_damagecauser()->CopyFrom(damageCauser);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_DIE);
	GetRoomRef()->BroadCast(sendBuffer);

	//죽으면 어떻게 할까?
	//GetRoomRef()->Remove(GetInfo());


}

void Creature::BindTarget(shared_ptr<Creature> target)
{
	_target = target;
	_info.set_targetid(_target->GetInfo().id());
}

void Creature::UnBindTarget()
{
	_target = nullptr;
	GetInfo().clear_targetid();

	//타겟을 향해 움직이게 한다?
	Protocol::Position currentPos = GetPos();

	currentPos.set_velocityx(0);
	currentPos.set_velocityy(0);
	currentPos.set_velocityz(0);
	SetPos(currentPos);

	Protocol::S_Move pkt;
	Protocol::ObjectInfo* sInfo = pkt.mutable_info();
	sInfo->CopyFrom(GetInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_MOVE);
	GetRoomRef()->BroadCast(sendBuffer);

	//

}

