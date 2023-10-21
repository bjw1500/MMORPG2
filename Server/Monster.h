#pragma once
#include "Creature.h"
#include "DataContents.h"
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
	virtual void UpdateCasting();

	virtual bool OnDead(Protocol::ObjectInfo damageCauser) override;
	FRewardData GetRandomReward();
	Position GetRandomPosition();

	virtual bool CanAttack();
	virtual void Attack(Protocol::Skill_ID skillId);
	virtual bool UseSkill1(Protocol::Skill_ID skillId);
	virtual bool SearchTarget();
	virtual void MoveTo(shared_ptr<Creature> target);
	virtual void MoveTo(Protocol::Position destination);
	void LookAt(Protocol::Position targetPos);

public:
	MonsterData _data;
protected:

	float _attackCoolTime = 0.0f;
	float _skillCoolTime1 = 0.0f;
	float _skillCoolTime2 = 0.0f;
	float _castingTime = 0.0f;

	float _nextPartol = 5.f;
	float _patrolTime = 0.0f;
	Protocol::Position _patrolPoint;
	bool  _isPatrol = false;
	
};

