// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGPlayer.h"
#include "MyAnimInstance.h"
#include "Creature.h"

ARPGPlayer::ARPGPlayer() 
{
	//Init();
}

void ARPGPlayer::OnDead()
{
	Anim->SetState(Protocol::CreatureState::Dead);
}
