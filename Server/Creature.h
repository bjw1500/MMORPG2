#pragma once

#include "GameObject.h"
#include "Protocol.pb.h"

class Creature : public GameObject
{
public:
	Creature();
	~Creature();
	
	void Ondamaged(Protocol::ObjectInfo damageCasuer, int32 damageAmount);
	void BindTarget(shared_ptr<Creature> target);
	void UnBindTarget();

	CreatureState GetState() { return GetInfo().state(); };
	void SetState(CreatureState state) { _info.set_state(state); }


public:
	shared_ptr<Creature> _target = nullptr;

};

