#pragma once
#include "Creature.h"
#include "GameObject.h"
class Monster : public Creature
{
public:
	Monster();
	~Monster();


	virtual void Update(float deltaTime) override;
	
	virtual void UpdateController();
	virtual void UpdateIdle();
	virtual void UpdateMove();
	virtual void UpdateAttack();
	virtual void UpdateDead();

	virtual bool CanAttack();
	virtual void UseSkill(Protocol::Skill_ID skillId);
	virtual bool SearchTarget();
	virtual void MoveTo(shared_ptr<Creature> target);

private:
	float _attackCoolTime = 0.0f;
};

