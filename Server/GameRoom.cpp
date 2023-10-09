#include "pch.h"
#include "GameObject.h"
#include "Creature.h"
#include "Player.h"
#include "Monster.h"
#include "GameRoom.h"
#include "GameSession.h"
#include <format>
#include "DataContents.h"
#include "DataManager.h"
#include "Item.h"
#include "Grux.h"
#include "Account.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"

atomic<int32> _objectId = 1;
GameRoomRef GRoom = make_shared<GameRoom>();


GameRoom::GameRoom()
{
	//PlayerRef newPlayer = CreatePlayer();
	//AddObject(newPlayer, NULL);
	cout << "GameRoom 생성" << endl;
}

GameRoom::~GameRoom()
{

	cout << "GameRoom 삭제" << endl;

}


//게임 업데이트는 메인 쓰레드에서 진행하므로 락이 필요 없다.
void GameRoom::Update(float deltaTime)
{
	_deltaTime = deltaTime;
	_autoSaveTime += deltaTime;
	if (_autoSaveTime > 30)
	{
		SaveAllPlayerDb();
	}


	for (auto player : _players)
	{
		player.second->Update(deltaTime);
	}
	for (auto monster : _monsters)
	{
		monster.second->Update(deltaTime);
	}
	for (auto object : _objects)
	{
		object.second->Update(deltaTime);
	}
}

void GameRoom::EnterPlayer(GameSessionRef session, int32 selectedNumber)
{
	lock_guard<mutex> gaurd(_lock);
	
	shared_ptr<PlayerDb> playerDb = session->GetMyAccount()->_players[selectedNumber];
	PlayerRef newPlayer = CreatePlayer(session, playerDb);

	//입장
	{
		Protocol::S_EnterRoom pkt;
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_ENTER_ROOM);
		session->Send(sendBuffer);
		printf("ID [%d] , Name [%s] 입장\n", newPlayer->GetPlayerDbId(), newPlayer->GetPlayerDb()->PlayerName.c_str());
		this_thread::sleep_for(1000ms);


		//생성한 캐릭터를 입장한 플레이어에게 보내준다.
		sendBuffer = ServerPacketHandler::Make_S_SpawnMyPlayer(newPlayer->GetInfo());
		session->Send(sendBuffer);
		int32 id = newPlayer->GetInfo().id();
		string name = newPlayer->GetInfo().name();

		//플레이어가 가지고 있던 아이템을 인벤토리에 넣어주는 패킷을 보내준다.
		newPlayer->RefreshInven();

		//플레이어의 퀘스트 목록을 보내준다.
		newPlayer->RefreshQuest();
	}
	

	//기존의 방에 있던 플레이어들과 오브젝트 정보를 보내준다.
	Protocol:: S_AddObjects pkt;
	Protocol::S_EquippedItem pkt2;
	{
		for (auto& player : _players)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = player.second->GetInfo();
			if (player.second->GetUsingWeapon() == nullptr)
				continue;

			pkt2.mutable_playerinfo()->CopyFrom(player.second->GetInfo());
			pkt2.mutable_iteminfo()->CopyFrom(*player.second->GetUsingWeapon());
		}

		for (auto& item : _items)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = item.second->GetInfo();

		}

		for (auto& monster : _monsters)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = monster.second->GetInfo();
		}
	}
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_ADDOBJECTS);
	session->Send(sendBuffer);

	sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt2, Protocol::S_EQUIPPED_ITEM);
	session->Send(sendBuffer);

	AddObject(newPlayer, session);
}

void GameRoom::EnterMonster(int x, int y, int z)
{
	lock_guard<mutex> gaurd(_lock);

	shared_ptr<Monster> newMonster = make_shared<Monster>();
	MonsterData data = GDataManager->GetMonsterData("Bear");
	newMonster->_data = data;
	newMonster->SetSkillData(&data.Stat);


	Protocol::ObjectInfo newInfo;

	newInfo.set_id(_objectId++);
	newInfo.set_type(ObjectType::MONSTER);
	string name = data.MonsterName + to_string(newInfo.id());
	newInfo.set_name(name);
	newInfo.set_state(CreatureState::Idle);
	newInfo.set_templateid(data.Id);

	printf("ID [%d] , Name [%s] 몬스터 생성\n", newInfo.id(), newInfo.name().c_str());

	Protocol::Position* position = newInfo.mutable_position();
	position->set_locationx(x);
	position->set_locationy(y);
	position->set_locationz(z);

	position->set_rotationx(0);
	position->set_rotationy(0);
	position->set_rotationz(-90);

	position->set_velocityx(0);
	position->set_velocityy(0);
	position->set_velocityz(0);

	Protocol::Stat* stat = newInfo.mutable_stat();
	stat->CopyFrom(data.Stat);

	newMonster->SetInfo(newInfo);
	AddObject(newMonster);
}

void GameRoom::EnterBoss()
{
	lock_guard<mutex> gaurd(_lock);

	shared_ptr<Grux> newMonster = make_shared<Grux>();
	MonsterData data = GDataManager->GetMonsterData("Grux");
	newMonster->_data = data;
	newMonster->SetSkillData(&data.Stat);


	Protocol::ObjectInfo newInfo;

	newInfo.set_id(_objectId++);
	newInfo.set_type(ObjectType::MONSTER);
	string name = data.MonsterName + to_string(newInfo.id());
	newInfo.set_name(name);
	newInfo.set_state(CreatureState::Idle);
	newInfo.set_templateid(data.Id);

	printf("ID [%d] , Name [%s] 몬스터 생성\n", newInfo.id(), newInfo.name().c_str());

	Protocol::Position* position = newInfo.mutable_position();
	position->set_locationx(5000.0);
	position->set_locationy(-4600.0);
	position->set_locationz(9300.0);

	position->set_rotationx(0);
	position->set_rotationy(0);
	position->set_rotationz(90);

	position->set_velocityx(0);
	position->set_velocityy(0);
	position->set_velocityz(0);

	Protocol::Stat* stat = newInfo.mutable_stat();
	stat->CopyFrom(data.Stat);

	newMonster->SetInfo(newInfo);
	AddObject(newMonster);
}

void GameRoom::EnterItem(Protocol::ItemInfo itemInfo, Protocol::Position location)
{
	lock_guard<mutex> gaurd(_lock);
	shared_ptr<Item> newItem = make_shared<Item>();

	Protocol::ObjectInfo newInfo;
	newInfo.set_id(_objectId++);
	newInfo.set_type(ObjectType::ITEM);
	string name = itemInfo.name() + to_string(newInfo.id());
	newInfo.set_name(name);
	newInfo.set_state(CreatureState::Idle);
	newInfo.set_templateid(itemInfo.templateid());
	//newInfo.mutable_iteminfo()->CopyFrom(itemInfo);

	printf("ID [%d] , Name [%s] 아이템 생성\n", newInfo.id(), newInfo.name().c_str());


	//위치 설정.
	Protocol::Position* position = newInfo.mutable_position();
	position->set_locationx(location.locationx());
	position->set_locationy(location.locationy());
	position->set_locationz(0);
	//position->set_locationz(location.locationz());
	position->set_rotationx(0);
	position->set_rotationy(0);
	position->set_rotationz(0);
	position->set_velocityx(0);
	position->set_velocityy(0);
	position->set_velocityz(0);

	newItem->SetInfo(newInfo);
	newItem->SetItemInfo(itemInfo);
	AddObject(newItem);
}

void GameRoom::Revive(GameSessionRef session, shared_ptr<PlayerDb> playerDb)
{
	lock_guard<mutex> gaurd(_lock);

	PlayerRef newPlayer = CreatePlayer(session, playerDb);

	//입장
	{
		Protocol::S_EnterRoom pkt;
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_ENTER_ROOM);
		session->Send(sendBuffer);
		printf("ID [%d] , Name [%s] 입장\n", newPlayer->GetPlayerDbId(), newPlayer->GetPlayerDb()->PlayerName.c_str());
		this_thread::sleep_for(1000ms);

		//생성한 캐릭터를 입장한 플레이어에게 보내준다.
		sendBuffer = ServerPacketHandler::Make_S_SpawnMyPlayer(newPlayer->GetInfo());
		session->Send(sendBuffer);
		int32 id = newPlayer->GetInfo().id();
		string name = newPlayer->GetInfo().name();

		//플레이어가 가지고 있던 아이템을 인벤토리에 넣어주는 패킷을 보내준다.
		Protocol::S_AddToInventory pkt2;
		for (auto item : newPlayer->GetInven()->GetItemList())
		{
			Protocol::ItemInfo* addItem = pkt2.add_items();
			*addItem = *item.second->GetItemInfo();
		}
		sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt2, S_ADD_TO_INVENTORY);
		session->Send(sendBuffer);
	}

	AddObject(newPlayer);
}



void GameRoom::AddObject(GameObjectRef gameObject, GameSessionRef ExceptSession)
{
	int32 id = gameObject->GetInfo().id();
	ObjectType type = gameObject->GetInfo().type();

	switch (type)
	{
	case PLAYER:
		_players[id] = static_pointer_cast<Player>(gameObject);
		break;
	case MONSTER:
		_monsters[id] = static_pointer_cast<Monster>(gameObject);
		break;
	case ITEM:
		_items[id] = static_pointer_cast<Item>(gameObject);
		break;
	case UNKNOWN:
		_objects[id] = gameObject;
	default:
		break;
	}

	gameObject->SetRoom(GetRoomRef());

	//새로운 오브젝트가 추가되면 방 안에 있는 다른 플레이어들에게 패킷 보내기
	{
		Protocol::S_AddObjects pkt;
		Protocol::ObjectInfo* info = pkt.add_objects();
		*info = gameObject->GetInfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_ADDOBJECTS);
		BroadCast(sendBuffer, ExceptSession);
	}

	//플레이어를 스폰하고, 그 플레이어가 장비를 끼고 있으면 끼게 해준다.
	if (type == PLAYER)
	{
		shared_ptr<Player> player = static_pointer_cast<Player>(gameObject);
		if (player->GetUsingWeapon() == nullptr)
			return;

		Protocol::S_EquippedItem pkt2;
		pkt2.mutable_playerinfo()->CopyFrom(player->GetInfo());
		pkt2.mutable_iteminfo()->CopyFrom(*player->GetUsingWeapon());
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt2, Protocol::S_EQUIPPED_ITEM);
		BroadCast(sendBuffer);
	}
}

PlayerRef GameRoom::CreatePlayer(GameSessionRef session, shared_ptr<PlayerDb> playerDb)
{
	shared_ptr<Player> newPlayer = make_shared<Player>();
	
	Protocol::ObjectInfo newInfo;
	newInfo.set_id(_objectId++);
	newInfo.set_templateid(playerDb->TemplatedId);
	newInfo.set_type(ObjectType::PLAYER);
	string name = "Player_" + session->GetMyAccount()->GetAccountName();
	newInfo.set_name(name);


	FCharacterData data = GDataManager->GetCharacterData(playerDb->TemplatedId);
	newInfo.mutable_stat()->mutable_skills()->CopyFrom(data.Stat.skills());
	//printf("ID [%d] , Name [%s] 캐릭터 생성\n", newInfo.id(), newInfo.name().c_str());
	newPlayer->SetInfo(newInfo);
	newPlayer->_session = session;
	session->SetMyPlayer(newPlayer);
	newPlayer->SetPlayerDb(playerDb);
	newPlayer->SetSpeed(GDataManager->GetCharacterData(1).Stat.movespeed());
	newPlayer->SetSkillData(&data.Stat);
	

	//플레이어가 가지고 있던 아이템 정보를 넣어준다.
	for (auto item : playerDb->Items)
	{
		shared_ptr<Item> loadItem = Item::MakeItem(item);
		newPlayer->GetInven()->AddItem(loadItem);
		if (loadItem->GetIsEquipped() == true)
			newPlayer->SetUsingWeapon(loadItem->GetItemInfo());
	}

	return newPlayer;
}


void GameRoom::HandleMove(Protocol::C_Move& movePacket)
{
	lock_guard<mutex> lock(_lock);
	Protocol::ObjectInfo* info = movePacket.mutable_info();

	
	//날아온 패킷의 정보를 서버에 저장된 플레이어의 정보에 그대로 저장.
	PlayerRef player = static_pointer_cast<Player>(FindObjectById(info->id()));
	if (player == nullptr)
		return;
	player->SetPos(info->position());


	//다른 플레이어들에게 업데이트 패킷 보내기
	Protocol::S_Move pkt;
	Protocol::ObjectInfo* sInfo = pkt.mutable_info();
	sInfo->CopyFrom(player->GetInfo());
	SendBufferRef sendBuffer= ServerPacketHandler::MakeSendBuffer(pkt, S_MOVE);
	BroadCast(sendBuffer, player->_session);


}

void GameRoom::HandleSkill(Protocol::C_Skill& skillPacket)
{
	lock_guard<mutex> lock(_lock);
	PlayerRef player = static_pointer_cast<Player>(FindObjectById(skillPacket.info().id()));
	if (player == nullptr)
		return;

	//쿨타임 중이라면 스킬 사용 X
	bool ret = player->UseSkill(skillPacket.skillid());
	if (ret == false)
		return;
	

	//스킬을 사용했다는 패킷을 보내줄 것인데, 사용한 사람한테는 스킵하자.
	Protocol::S_Skill pkt;
	Protocol::ObjectInfo* info = pkt.mutable_info();
	info->CopyFrom(skillPacket.info());
	pkt.set_skillid(skillPacket.skillid());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_SKILL);
	BroadCast(sendBuffer);
}

void GameRoom::HandleChangedHP(Protocol::C_ChangedHP& changedPacket)
{
	//lock_guard<mutex> lock(_lock);

	shared_ptr<Creature> player = static_pointer_cast<Creature>(FindObjectById(changedPacket.target().id()));
	shared_ptr<Creature> damageCasuer = static_pointer_cast<Creature>(FindObjectById(changedPacket.damagecauser().id()));

	if (player == nullptr)
		return;
	if (damageCasuer == nullptr)
		return;

	//printf("%s가 %s한테 공격당했습니다! %d! \n", player->GetInfo().name().c_str(), damageCasuer->GetInfo().name().c_str(), changedPacket.damageamount());

	player->OnDamaged(damageCasuer->GetInfo(), changedPacket.damageamount());

	Protocol::S_ChangedHP pkt;
	Protocol::ObjectInfo* pkt_target = pkt.mutable_target();
	Protocol::ObjectInfo* pkt_damageCauser = pkt.mutable_damagecauser();

	pkt_target->CopyFrom(player->GetInfo());
	pkt_damageCauser->CopyFrom(damageCasuer->GetInfo());
	pkt.set_damageamount(changedPacket.damageamount());

	//printf("%s가 %s한테 공격당했습니다! %d! \n", pkt_target->name().c_str(), pkt_damageCauser->name().c_str(), changedPacket.damageamount());
	//printf("%s의 남은 체력 %d! \n", pkt_target->name().c_str(), pkt_damageCauser->stat().hp());

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_CHANGEDHP);

	BroadCast(sendBuffer);


}

void GameRoom::HandleChat(Protocol::C_Chat& chatPacket)
{
	lock_guard<mutex> gaurd(_lock);

	Protocol::S_Chat pkt;

	PlayerRef player = static_pointer_cast<Player>(FindObjectById(chatPacket.id()));
	if (player == nullptr)
	{
		cout << "HandleChat Error 없는 플레이어." << endl;
		return;
	}

	cout << chatPacket.msg() << endl;

	pkt.set_id(chatPacket.id());
	pkt.set_name(player->GetInfo().name());
	string sendMsg = std::format("[{}]:{}", player->GetInfo().name(), chatPacket.msg());
	pkt.set_msg(sendMsg);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_CHAT);
	BroadCast(sendBuffer);
}

void GameRoom::HandlePickUpItem(Protocol::C_PickUpItem& PickPacket)
{
	lock_guard<mutex> gaurd(_lock);

	if (_items.contains(PickPacket.pickitem().id()) == false)
	{
		cout << "잘못된 아이템 ID[" << PickPacket.pickitem().id() << " ] 입니다." << endl;
		return;
	}
	
	shared_ptr<Player> player = static_pointer_cast<Player>(FindObjectById(PickPacket.info().id()));
	if (player == nullptr)
	{
		cout << "Handle Pick Up Item Error "<< PickPacket.info().name()  << endl;
		return;
	}

	//먹은 아이템을 필드에서 지워준다.
	auto value =	_items.find(PickPacket.pickitem().id());
	shared_ptr<Item> item = value->second;
	_items.erase(PickPacket.pickitem().id());

	player->PickItem(item);
}

void GameRoom::HandleMonsterMoveByClient(Protocol::C_MonsterMove& movePacket)
{
	lock_guard<mutex> gaurd(_lock);

	MonsterRef monster = static_pointer_cast<Monster>(FindObjectById(movePacket.info().id()));
	monster->SetInfo(*movePacket.mutable_info());
	monster->SetState(CreatureState::Idle);

	Protocol::S_Move pkt;
	Protocol::ObjectInfo* sInfo = pkt.mutable_info();
	sInfo->CopyFrom(monster->GetInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_MOVE);
	GetRoomRef()->BroadCast(sendBuffer);
}

void GameRoom::HandleEquippedItem(Protocol::C_EquippedItem& packet)
{
	lock_guard<mutex> gaurd(_lock);

	shared_ptr<Player> player = static_pointer_cast<Player>(FindObjectById(packet.playerinfo().id()));
	if (player == nullptr)
	{
		cout << "Handle Equipped Item Error " << packet.playerinfo().name() << endl;
		return;
	}

	shared_ptr<Item> item = player->GetInven()->GetItem(packet.iteminfo().databaseid());
	if (item == nullptr)
		return;

	player->EquippedItem(item);

}

void GameRoom::HandleUseItem(Protocol::C_UseItem& packet)
{
	lock_guard<mutex> gaurd(_lock);

	shared_ptr<Player> player = static_pointer_cast<Player>(FindObjectById(packet.playerinfo().id()));
	if (player == nullptr)
	{
		cout << "Handle Equipped Item Error " << packet.playerinfo().name() << endl;
		return;
	}

	shared_ptr<Item> item = player->GetInven()->GetItem(packet.iteminfo().databaseid());
	if (item == nullptr)
		return;

	player->UseItem(item);

}

void GameRoom::Remove(Protocol::ObjectInfo info)
{
	lock_guard<mutex> gaurd(_lock);

	Protocol::ObjectType type = info.type();

	switch (type)
	{
		case Protocol::PLAYER:
		{
			_players.erase(info.id());
		}
			break;
		case Protocol::MONSTER:
		{
			_monsters.erase(info.id());
		}
			break;
		case Protocol::ITEM:
			break;
		case Protocol::UNKNOWN:
			break;
		default:
			break;
	}
	//TODO 제거 패킷 보내기

	Protocol::S_Despawn pkt;
	Protocol::ObjectInfo* pktinfo = pkt.add_objects();
	*pktinfo = info;
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_DEWSPAWN);

	BroadCast(sendBuffer);


}

void GameRoom::BroadCast(SendBufferRef sendBuffer, GameSessionRef exceptSession)
{
	for (auto& item : _players)
	{
		PlayerRef player = item.second;
		if (player->_session == exceptSession)
			continue;
		player->_session->Send(sendBuffer);
	}
}

GameObjectRef GameRoom::FindObjectById(int32 id)
{
	if (_players.find(id) == _players.end())
	{
		if (_monsters.find(id) == _monsters.end())
		{
			if (_items.find(id) == _items.end())
			{
				//printf("FindObjectById Error! ID[%d] 없는 ID입니다.\n", id);
				return nullptr;
			}
			return _items[id];
		}
		return _monsters[id];
	}

	return _players[id];
}

void GameRoom::SaveAllPlayerDb()
{
	_autoSaveTime = 0;
	DBConnection* db = GDBConnectionPool->Pop();
	
	for (auto player : _players)
	{
		db->SavePlayerInfo(player.second);
	}

	//db->SavePlayerInfo(_myPlayer);
	GDBConnectionPool->Push(db);

}


