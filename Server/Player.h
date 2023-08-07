#pragma once

#include "Creature.h"


class Player : public Creature
{
public:
	Player();
	~Player();

public:
	GameSessionRef _session;
};

