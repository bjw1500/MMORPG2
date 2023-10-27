#include "pch.h"
#include "Creature.h"
#include "GameRoom.h"
#include "DataManager.h"
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

bool Creature::OnDead(Protocol::ObjectInfo damageCauser)
{
	if (GetState() == CreatureState::Dead)
		return false;

	printf("%s가 %s의 공격을 받아 사망했습니다.\n", GetInfo().name().c_str(), damageCauser.name().c_str());
	SetState(CreatureState::Dead);

	//플레이어들한테 죽음 통지
	Protocol::S_Die pkt;
	pkt.mutable_target()->CopyFrom(GetInfo());
	pkt.mutable_damagecauser()->CopyFrom(damageCauser);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_DIE);
	GetRoomRef()->BroadCast(sendBuffer);

	//죽으면 어떻게 할까?
	GetRoomRef()->Remove(GetInfo());
	return true;
}

void Creature::Update(float deltaTime)
{
	GameObject::Update(deltaTime);
	UpdateSkillCoolTime(deltaTime);

}

void Creature::UpdateSkillCoolTime(float deltaTime)
{
	if (_skills.size() == 0)
		return;

	for (auto& skill : _skills)
	{
		//스킬 사용이 가능할 때는 무시,
		if (skill.second.CanUse == true)
			continue;
		skill.second.SkillCoolTime += deltaTime;

		if (skill.second.SkillCoolTime > skill.second.CoolTime)
		{
			skill.second.SkillCoolTime = 0;
			skill.second.CanUse = true;
		}
	}
}

bool Creature::UseSkill(int32 id)
{

	if (_skills[id].CanUse == false)
		return false;

	_skills[id].CanUse = false;
	_skills[id].SkillCoolTime = 0;
	return true;
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
}

int32 Creature::GetAttackIndex()
{
	int32 ReturnIndex = _attackIndex;
	_attackIndex = (_attackIndex + 1) % 4;
	return ReturnIndex;
}

void Creature::SetSkillData(Protocol::Stat* stat)
{
	for (auto skill : stat->skills())
	{
		FSkillData data = GDataManager->GetSkillData(skill);
		_skills[data.SkillType_ID] = data;
	}
}

