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
#include "DBConnection.h"
#include "DBConnectionPool.h"

Monster::Monster() : Creature()
{
}

Monster::~Monster()
{
}

void Monster::Update(float deltaTime)
{
	Creature::Update(deltaTime);

	if(_attackCoolTime <=10)
		_attackCoolTime += deltaTime;
	if (GetState() == Casting)
		_castingTime += deltaTime;
	if (GetState() == Idle)
		_patrolTime += deltaTime;


	UpdateController();
}

void Monster::UpdateController()
{
	switch (GetState())
	{
	case Idle:
		UpdateIdle();
		break;
	case Move:
		UpdateMove();
		break;
	case CreatureState::Attack:
		UpdateAttack();
		break;
	case Dead:
		UpdateDead();
		break;
	case Casting:
		UpdateCasting();
		break;
	default:
		break;
	}

	
}

void Monster::UpdateIdle()
{
	//대기 상태일 때는 타겟을 찾는다.

	bool ret = SearchTarget();
	if (ret == true)
		SetState(CreatureState::Move);

	//순찰해주기
	if (_patrolTime >= _nextPartol)
	{
		_patrolPoint = GetRandomPosition();
		_patrolTime = 0;
		_isPatrol = true;
		SetState(CreatureState::Move);
	}

}

void Monster::UpdateMove()
{
	if (_isPatrol == true)
	{
		//순찰 지점에 도착했는지 확인.
		float dis = GetDistanceFromTarget(_patrolPoint);
		if (dis <= 30.f)
		{
			_isPatrol = false;
			SetState(CreatureState::Idle);
			return;
		}

		bool ret = SearchTarget();
		if (ret == true)
			_isPatrol = false;


		MoveTo(_patrolPoint);
		return;
	}


	//타겟이 중간에 죽거나, 사라졌으면, Idle 상태로 전환.
	if (_target == nullptr)
	{
		SetState(CreatureState::Idle);
		return;
	}else if (_target->GetState() == CreatureState::Dead)
	{
		UnBindTarget();
		SetState(CreatureState::Idle);
		return;
	}

	//타겟이 존재한다면 거리 재기
	float dis = GetDistanceFromTarget(_target->GetPos());
	if (dis > GetInfo().stat().searchrange())
	{
		//만약 추적중에 상대방이 탐색거리를 벗어났다면, 
		UnBindTarget();
		SetState(CreatureState::Idle);
		return;
	}

	MoveTo(_target);
	//타깃과의 거리가 공격 거리 안으로 들어오면 공격한다.
	if (CanAttack() == true)
	{
		SetState(CreatureState::Attack);
	}

}

void Monster::UpdateAttack()
{	
	if (CanAttack() == true)
	{
		Attack(Protocol::Skill_ID::ATTACK);
	}else 
	{
		//만약 상대방이 공격 거리에서 벗어났다면, 
		SetState(CreatureState::Move);
	}
}

void Monster::UpdateDead()
{
}

void Monster::UpdateCasting()
{
	if (_castingTime > 5.0f)
	{
		_castingTime = 0;
		SetState(Idle);

	}

}

bool Monster::OnDead(Protocol::ObjectInfo damageCauser)
{
	bool ret = Creature::OnDead(damageCauser);
	if (ret == false)
		return false;

	//죽인 플레이어한테 경험치를 넣어준다.
	shared_ptr<Player> player = static_pointer_cast<Player>(GetRoomRef()->FindObjectById(damageCauser.id()));

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, 99);
	player->GetInven()->AddGold(dis(gen));
	player->RefreshInven();

	//죽으면 아이템을 떨구게 한다.
	FRewardData reward = GetRandomReward();
	Protocol::ItemInfo rewardItem = GDataManager->GetItemData(reward.Id);

	//떨군 아이템을 플레이어들한테 통지해준다.
	GetRoomRef()->EnterItem(rewardItem, GetPos());

	//만약 죽은 몬스터가 플레이어의 퀘스트 목표였다면?
	player->CheckQuestTarget(GetTemplatedId());

	return true;
}

FRewardData Monster::GetRandomReward()
{
	// 시드값을 얻기 위한 random_device 생성.
	std::random_device rd;
	// random_device 를 통해 난수 생성 엔진을 초기화 한다.
	std::mt19937 gen(rd());
	// 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.
	std::uniform_int_distribution<int> dis(0, 100);
	int rand = dis(gen);
	int sum = 0;
	for (FRewardData reward : _data.RewardDatas)
	{
		sum += reward.ItemDropRate;
		if (rand <= sum)
		{
			return reward;
			break;
		}
	}
	return FRewardData();
}

Position Monster::GetRandomPosition()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(-1000, 1000);
	int rand = dis(gen);
	Protocol::Position randomPosition;
	randomPosition.set_locationx(GetPos().locationx() + rand);
	rand = dis(gen);
	randomPosition.set_locationy(GetPos().locationy() + rand);
	randomPosition.set_locationz(GetPos().locationz());

	return randomPosition;
}



bool Monster::CanAttack()
{
	if (_target == nullptr ||_target->GetState() == CreatureState::Dead)
	{
		UnBindTarget();
		SetState(CreatureState::Idle);
		return false;
	}


	float dis = GetDistanceFromTarget(_target->GetPos());
	if (dis <= GetInfo().stat().attackrange())
	{
		return true;
	}
	return false;
}

void Monster::Attack(Protocol::Skill_ID skillId)
{

	if (_attackCoolTime < 1)
		return;
	else
		_attackCoolTime = 0;

	Protocol::S_Skill pkt;
	Protocol::ObjectInfo* monster = pkt.mutable_info();
	pkt.set_attackindex(0);
	monster->CopyFrom(GetInfo());
	pkt.set_skillid(skillId);
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_SKILL);
	GetRoomRef()->BroadCast(sendBuffer);


	//string text = std::format("{}가 {}를 {}공격력으로 공격합니다.", GetInfo().name(), _target->GetInfo().name(), _target->GetInfo().stat().damage());
	//cout << text << endl;
}

bool Monster::UseSkill1(Protocol::Skill_ID skillId)
{
	return true;
}

bool Monster::SearchTarget()
{
	GameRoomRef myRoom = GetRoomRef();
	if (myRoom == nullptr)
		return false;

	for (auto player : myRoom->_players)
	{
		float dis = GetDistanceFromTarget(player.second->GetPos());
		if (dis <= GetInfo().stat().searchrange())
		{
			BindTarget(player.second);
			return true;
		}
	}
	UnBindTarget();
	return false;
}

void Monster::MoveTo(shared_ptr<Creature> target)
{
	//타겟을 향해 움직이게 한다?
	Protocol::Position currentPos = GetPos();
	Protocol::Position targetPos = target->GetPos();
	float moveSpeed = GetInfo().stat().movespeed() * GetRoomRef()->_deltaTime;

	//왼쪽
	if (currentPos.locationx() > targetPos.locationx())
	{
		currentPos.set_locationx(currentPos.locationx() - moveSpeed);
	}
	//오른쪽
	if (currentPos.locationx() < targetPos.locationx())
	{
		currentPos.set_locationx(currentPos.locationx() + moveSpeed);
	}
	
	//뒤
	if (currentPos.locationy() > targetPos.locationy())
	{
		currentPos.set_locationy(currentPos.locationy() - moveSpeed);
	}

	//앞
	if (currentPos.locationy() < targetPos.locationy())
	{
		currentPos.set_locationy(currentPos.locationy() + moveSpeed);
	}

	currentPos.set_velocityx(100);
	currentPos.set_velocityy(100);
	currentPos.set_velocityz(1);

	SetPos(currentPos);
	LookAt(targetPos);
	Protocol::S_Move pkt;
	Protocol::ObjectInfo* sInfo = pkt.mutable_info();
	sInfo->CopyFrom(GetInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_MOVE);
	GetRoomRef()->BroadCast(sendBuffer);
}

void Monster::MoveTo(Protocol::Position destination)
{
	//타겟을 향해 움직이게 한다?
	Protocol::Position currentPos = GetPos();
	float moveSpeed = GetInfo().stat().movespeed() * GetRoomRef()->_deltaTime;



	//왼쪽
	if (currentPos.locationx() > destination.locationx())
	{
		currentPos.set_locationx(currentPos.locationx() - moveSpeed);
	}
	//오른쪽
	if (currentPos.locationx() < destination.locationx())
	{
		currentPos.set_locationx(currentPos.locationx() + moveSpeed);
	}

	//뒤
	if (currentPos.locationy() > destination.locationy())
	{
		currentPos.set_locationy(currentPos.locationy() - moveSpeed);
	}

	//앞
	if (currentPos.locationy() < destination.locationy())
	{
		currentPos.set_locationy(currentPos.locationy() + moveSpeed);
	}

	currentPos.set_velocityx(100);
	currentPos.set_velocityy(100);
	currentPos.set_velocityz(1);
	SetPos(currentPos);
	LookAt(destination);
	Protocol::S_Move pkt;
	Protocol::ObjectInfo* sInfo = pkt.mutable_info();
	sInfo->CopyFrom(GetInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_MOVE);
	GetRoomRef()->BroadCast(sendBuffer);
}

void Monster::LookAt(Protocol::Position targetPos)
{
	float dirX = targetPos.locationx() - GetPos().locationx();
	float dirY = targetPos.locationy() - GetPos().locationy();
	double target_yaw = std::atan2(dirY, dirX);
	double yawInDegrees = target_yaw * 180.0 / 3.14;
	Protocol::Position currentPos = GetPos();
	currentPos.set_rotationz(yawInDegrees);
	SetPos(currentPos);
}


