#pragma once
#include "Protocol.pb.h"

class PlayerDb;

class GameRoom : public enable_shared_from_this<GameRoom>
{
public:
	GameRoom();
	~GameRoom();

	void Update(float deltaTime);


	//InGame
	void EnterPlayer(GameSessionRef session, int32 selectedNumber = 0);
	void EnterMonster(int x, int y, int z);
	void EnterBoss();
	void EnterItem(Protocol::ItemInfo itemInfo, Protocol::Position location);
	void Revive(GameSessionRef session, shared_ptr<PlayerDb> playerDb);
	void AddObject(GameObjectRef session, GameSessionRef ExceptSession = nullptr);

public:
	PlayerRef CreatePlayer(GameSessionRef session, shared_ptr<PlayerDb> playerDb);
	GameRoomRef GetRoomRef() {
		return shared_from_this();
	}


	//Handle
	void HandleMove(Protocol::C_Move& movePacket);
	void HandleSkill(Protocol::C_Skill& skillPacket);
	void HandleChangedHP(Protocol::C_ChangedHP& changedPacket);
	void HandleChat(Protocol::C_Chat& chatPacket);
	void HandlePickUpItem(Protocol::C_PickUpItem& Pickpacket);
	void HandleMonsterMoveByClient(Protocol::C_MonsterMove& movePacket);
	void HandleEquippedItem(Protocol::C_EquippedItem& packet);
	void HandleUseItem(Protocol::C_UseItem& packet);

	void Remove(Protocol::ObjectInfo info);
	void BroadCast(SendBufferRef sendBuffer, GameSessionRef exceptSession = nullptr);


	//Find
	GameObjectRef FindObjectById(int32 id);

	//DataBase
	void SaveAllPlayerDb();

public:
	float _deltaTime;
	float _autoSaveTime = 0;
	mutex _lock;
	map<int32, shared_ptr<Player>> _players;
	map<int32, MonsterRef> _monsters;
	map<int32, GameObjectRef> _objects;
	map<int32, shared_ptr<class Item>> _items;
};

extern 	atomic<int32> _objectId;
extern GameRoomRef GRoom;

