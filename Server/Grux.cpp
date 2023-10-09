#include "pch.h"
#include "Monster.h"
#include "GameRoom.h"
#include "Player.h"
#include "ServerPacketHandler.h"
#include "DataManager.h"
#include "DataContents.h"
#include <format>
#include "Protocol.pb.h"
#include <random>
#include "Grux.h"

Grux::Grux() : Monster()
{
}

Grux::~Grux()
{
}

void Grux::Update(float deltaTime)
{
	Monster::Update(deltaTime);
	if (_skillCoolTime1 < 10)
		_skillCoolTime1 += deltaTime;
	if (_skillCoolTime2 < 10)
		_skillCoolTime2 += deltaTime;

}

void Grux::UpdateAttack()
{
	if (CanAttack() == true)
	{
		if (UseSkill(Skill_ID::SKILL_3) == true)
			return;
		if (UseSkill(Skill_ID::SKILL_1) == true)
			return;

		Attack(Protocol::Skill_ID::ATTACK);
	}
	else
	{
		//만약 상대방이 공격 거리에서 벗어났다면, 
		SetState(CreatureState::Idle);
	}
}

void Grux::UpdateMove()
{
	//AI 이동은 언리얼 AI를 활용하자.
//그래야 네비게이션 활용이 가능.

//타겟이 중간에 죽거나, 사라졌으면, Idle 상태로 전환.
	if (_target == nullptr)
	{
		SetState(CreatureState::Idle);
		return;
	}
	else if (_target->GetState() == CreatureState::Dead)
	{
		UnBindTarget();
		SetState(CreatureState::Idle);
		return;
	}

	float dis = GetDistanceFromTarget(_target->GetPos());


	if (dis > GetInfo().stat().searchrange())
	{
		//만약 추적중에 상대방이 탐색거리를 벗어났다면, 
		//if (UseSkill1(Skill_ID::SKILL_1) == true)
		//	return;

		UnBindTarget();
		SetState(CreatureState::Idle);
		return;
	}

	//추격은 가능하지만 타겟이 너무 멀면 스킬로 덮친다.

	MoveTo(_target);

	//타깃과의 거리가 공격 거리 안으로 들어오면 공격한다.
	if (CanAttack() == true)
	{
		SetState(CreatureState::Attack);
	}
	else
	{

		//공격 거리는 아니지만, 탐색 거리의 절반에 있을 때.
		if (dis > GetInfo().stat().searchrange() / 2)
		{
			//만약 상대방이 멀어지고 있다면, 돌진 스킬 사용.
			if (UseSkill(Skill_ID::SKILL_2) == true)
				return;
		}
	}
}

void Grux::Attack(Protocol::Skill_ID skillId)
{
	if (_attackCoolTime < 2)
		return;
	else
		_attackCoolTime = 0;

	Protocol::S_Skill pkt;
	Protocol::ObjectInfo* monster = pkt.mutable_info();
	monster->CopyFrom(GetInfo());
	pkt.set_skillid(skillId);
	pkt.set_attackindex(GetAttackIndex());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_SKILL);
	GetRoomRef()->BroadCast(sendBuffer);
}

bool Grux::UseSkill(int32 id)
{
	bool ret = Creature::UseSkill(id);
	if (ret == false)
		return false;

	_attackCoolTime = 0;

	SetState(CreatureState::Casting);
	Protocol::S_Skill pkt;
	Protocol::ObjectInfo* monster = pkt.mutable_info();
	monster->CopyFrom(GetInfo());
	pkt.set_skillid(id);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_SKILL);
	GetRoomRef()->BroadCast(sendBuffer);
	return true;
}

