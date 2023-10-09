#pragma once
#include "Monster.h"
class Grux :
    public Monster
{
public:
	Grux();
	~Grux();

	virtual void Update(float deltaTime) override;
	virtual void UpdateAttack() override;
	virtual void UpdateMove();
	

	virtual void Attack(Protocol::Skill_ID skillId);
	virtual bool UseSkill(int32 id) override;



};

