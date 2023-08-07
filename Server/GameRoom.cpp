#include "pch.h"
#include "GameObject.h"
#include "Creature.h"
#include "Player.h"
#include "Monster.h"
#include "GameRoom.h"
#include "GameSession.h"
#include <format>
#include "Account.h"

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

void GameRoom::Update(float deltaTime)
{

	_deltaTime = deltaTime;
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

void GameRoom::EnterPlayer(GameSessionRef session)
{

	lock_guard<mutex> gaurd(_lock);

	PlayerRef newPlayer = CreatePlayer(session);
	newPlayer->_session = session;
	session->SetMyPlayer(newPlayer);

	{
		int32 id = newPlayer->GetInfo().id();
		string name = newPlayer->GetInfo().name();
		Protocol::S_EnterRoom pkt;
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_ENTER_ROOM);
		session->Send(sendBuffer);
		printf("ID [%d] , Name [%s] 입장\n", id, name.c_str());
	}

	this_thread::sleep_for(1000ms);

	{
		//생성한 캐릭터를 입장한 플레이어에게 보내준다.
		SendBufferRef sendBuffer = ServerPacketHandler::Make_S_SpawnMyPlayer(newPlayer->GetInfo());
		session->Send(sendBuffer);
		int32 id = newPlayer->GetInfo().id();
		string name = newPlayer->GetInfo().name();

	}



	{
		Protocol:: S_AddObjects pkt;
		//기존의 방에 있던 플레이어들과 오브젝트 정보를 보내준다.
		for (auto& player : _players)
		{
			 Protocol::ObjectInfo* info = pkt.add_objects();
			 *info = player.second->GetInfo();
		}

		for (auto& object : _objects)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = object.second->GetInfo();

		}

		for (auto& monster : _monsters)
		{
			Protocol::ObjectInfo* info = pkt.add_objects();
			*info = monster.second->GetInfo();
		}


		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_ADDOBJECTS);
		session->Send(sendBuffer);

	}



	AddObject(newPlayer, session);
}

void GameRoom::EnterMonster()
{
	shared_ptr<Monster> newMonster = make_shared<Monster>();
	ObjectInfo newInfo;
	newInfo.set_id(_objectId++);
	newInfo.set_type(ObjectType::MONSTER);
	string name = "Monster_" + newInfo.id();
	newInfo.set_name(name);
	newInfo.set_state(CreatureState::Idle);

	printf("ID [%d] , Name [%s] 몬스터 생성\n", newInfo.id(), newInfo.name().c_str());

	Position* position = newInfo.mutable_position();
	position->set_locationx(0);
	position->set_locationy(0);
	position->set_locationz(100);

	position->set_rotationx(0);
	position->set_rotationy(0);
	position->set_rotationz(0);

	position->set_velocityx(0);
	position->set_velocityy(0);
	position->set_velocityz(0);

	//TODO 나중에 데이터 시트 연동해서 몬스터 정보를 로드해주기.
	//하지만 지금은 데이터 시트가 없으므로 그냥 직접 값을 넣어준다.
	Stat* stat = newInfo.mutable_stat();
	stat->set_hp(100);
	stat->set_maxhp(100);
	stat->set_damage(10);
	stat->set_level(1);
	stat->set_name("Bear");
	stat->set_searchrange(1000);
	stat->set_attackrange(300);
	stat->set_movespeed(100);

	newMonster->SetInfo(newInfo);
	AddObject(newMonster);
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
	case UNKNOWN:
		_objects[id] = gameObject;
	default:
		break;
	}

	//printf("ID[%d], Name[%s]가 GameRoom에 추가되었습니다.",
	//	gameObject->GetInfo().id(), gameObject->GetInfo().name());
	gameObject->SetRoom(GetRoomRef());



	//새로운 오브젝트가 추가되면 방 안에 있는 다른 플레이어들에게 패킷 보내기
	{
		Protocol::S_AddObjects pkt;
		Protocol::ObjectInfo* info = pkt.add_objects();
		*info = gameObject->GetInfo();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_ADDOBJECTS);
		BroadCast(sendBuffer, ExceptSession);
	}
}

PlayerRef GameRoom::CreatePlayer(GameSessionRef session)
{
	shared_ptr<Player> newPlayer = make_shared<Player>();
	
	ObjectInfo newInfo;
	newInfo.set_id(_objectId++);
	newInfo.set_type(ObjectType::PLAYER);
	string name = "Player_" + session->GetMyAccount()->_accountName;
	newInfo.set_name(name);

	printf("ID [%d] , Name [%s] 캐릭터 생성\n", newInfo.id(), newInfo.name().c_str());

	Position* position = newInfo.mutable_position();
	position->set_locationx(0);
	position->set_locationy(0);
	position->set_locationz(100);

	position->set_rotationx(0);
	position->set_rotationy(0);
	position->set_rotationz(0);

	position->set_velocityx(0);
	position->set_velocityy(0);
	position->set_velocityz(0);
	Stat* stat = newInfo.mutable_stat();
	stat->set_hp(100);
	stat->set_maxhp(100);
	stat->set_damage(10);
	stat->set_level(1);
	stat->set_name("Player");

	//나중에 Json 연동해서 몬스터 정보 관리해주기

	newPlayer->SetInfo(newInfo);

	return newPlayer;
}





void GameRoom::HandleMove(Protocol::C_Move movePacket)
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

void GameRoom::HandleSkill(Protocol::C_Skill skillPacket)
{
	lock_guard<mutex> lock(_lock);
	PlayerRef player = static_pointer_cast<Player>(FindObjectById(skillPacket.info().id()));

	//TODO
	//나중에 데이터 시트를 만들게 되면 서버에서 쿨타임 확인도 해주자.
	//하지만 지금은 없으므로 일단 스킵.

	//생각해볼 것.
	//굳이 C_Skill 과 S_Skill을 나누어 줘야 할까?
	//그냥 패킷을 넘겨 받고, 그걸 그대로 넘겨주면 좀 더 효율적이지 않을까?
	
	//스킬을 사용했다는 패킷을 보내줄 것인데, 사용한 사람한테는 스킵하자.
	Protocol::S_Skill pkt;
	Protocol::ObjectInfo* info = pkt.mutable_info();
	info->CopyFrom(skillPacket.info());
	pkt.set_skillid(skillPacket.skillid());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_SKILL);
	BroadCast(sendBuffer, player->_session);
}

void GameRoom::HandleChangedHP(Protocol::C_ChangedHP changedPacket)
{
	lock_guard<mutex> lock(_lock);

	shared_ptr<Creature> player = FindObjectById(changedPacket.target().id());
	shared_ptr<Creature> damageCasuer = FindObjectById(changedPacket.damagecauser().id());

	if (player == nullptr)
		return;
	if (damageCasuer == nullptr)
		return;

	//printf("%s가 %s한테 공격당했습니다! %d! \n", player->GetInfo().name().c_str(), damageCasuer->GetInfo().name().c_str(), changedPacket.damageamount());

	player->Ondamaged(damageCasuer->GetInfo(), changedPacket.damageamount());

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

void GameRoom::HandleChat(Protocol::C_Chat chatPacket)
{
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

void GameRoom::Remove(Protocol::ObjectInfo info)
{
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

shared_ptr<Creature> GameRoom::FindObjectById(int32 id)
{
	if (_players.find(id) == _players.end())
	{
		if (_monsters.find(id) == _monsters.end())
		{
			printf("FindPlayerById Error! ID[%d] 없는 ID입니다.\n", id);
			return nullptr;
		}
		return _monsters[id];
	}

	return _players[id];
}

