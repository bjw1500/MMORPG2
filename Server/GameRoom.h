#pragma once
#include "Protocol.pb.h"

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom();

	void Update(float deltaTime);


	//플레이어 입장
	void EnterPlayer(GameSessionRef session);
	void EnterMonster();
	void AddObject(GameObjectRef session, GameSessionRef ExceptSession = nullptr);

public:
	PlayerRef CreatePlayer(GameSessionRef session);
	GameRoomRef GetRoomRef() {
		return shared_from_this();
	}


	//Handle
	void HandleMove(Protocol::C_Move movePacket);
	void HandleSkill(Protocol::C_Skill skillPacket);
	void HandleChangedHP(Protocol::C_ChangedHP changedPacket);
	void HandleChat(Protocol::C_Chat chatPacket);

	void Remove(Protocol::ObjectInfo info);
	void BroadCast(SendBufferRef sendBuffer, GameSessionRef exceptSession = nullptr);


	//Find
	shared_ptr<Creature> FindObjectById(int32 id);

public:


	float _deltaTime;
	//플레이어 관리는 어떻게?
	mutex _lock;
	map<int32, shared_ptr<Player>> _players;
	map<int32, MonsterRef> _monsters;
	map<int32, GameObjectRef> _objects;


};

extern 	atomic<int32> _objectId;
extern GameRoomRef GRoom;

