#pragma once

#include "GameObject.h"
#include "DataContents.h"
#include "Protocol.pb.h"

class Creature : public GameObject
{
public:
	Creature();
	~Creature();
	
	void OnDamaged(Protocol::ObjectInfo damageCauser, int32 damageAmount);
	virtual bool OnDead(Protocol::ObjectInfo damageCauser);

	virtual void Update(float deltaTime) override;
	void UpdateSkillCoolTime(float deltaTime);
	virtual bool UseSkill(int32 id);

	void BindTarget(shared_ptr<Creature> target);
	void UnBindTarget();

	CreatureState GetState() { return GetInfo().state(); };
	void SetState(CreatureState state) {
		_info.set_state(state);
		UpdateInfo();
	}
	int32 GetAttackIndex();

	void SetSkillData(Protocol::Stat* stat);


public:
	shared_ptr<Creature> _target = nullptr;
	map<int32, FSkillData> _skills;
	int32 _attackIndex;
};

