#include "pch.h"
#include "Player.h"
#include "GameRoom.h"
#include "GameSession.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"
#include "DataManager.h"

Player::Player() : Creature()
{
	
	
}

Player::~Player()
{


}

void Player::Update(float deltaTime)
{
	Creature::Update(deltaTime);


}

void Player::PickItem(shared_ptr<Item> item)
{
	
	Additem(item);
	//먹었다고 플레이어한테 패킷 보내기
	Protocol::S_PickUpItem pkt;
	pkt.mutable_info()->CopyFrom(GetInfo());
	pkt.mutable_pickitem()->CopyFrom(item->GetInfo());
	pkt.mutable_iteminfo()->CopyFrom(*item->GetItemInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_PICKUPITEM);
	GetRoomRef()->BroadCast(sendBuffer);
}

void Player::EquippedItem(shared_ptr<Item>& item)
{
	//기존에 이미 아이템을 착용하고 있다면,
	if (_usingWeapon != nullptr)
	{
		GetInven()->GetItem(_usingWeapon->databaseid())->GetItemInfo()->set_isequipped(false);
		SetUsingWeapon(item->GetItemInfo());
	}
	//장비의 공격력 만큼 캐릭터의 데미지를 올려준다.
	item->GetItemInfo()->set_isequipped(true);
	SetDamage(item->GetDamage());

		//아이템을 장착했다면 다른 플레이어들에게 통지를 해줘야한다.
	Protocol::S_EquippedItem pkt;
	pkt.mutable_playerinfo()->CopyFrom(GetInfo());
	pkt.mutable_iteminfo()->CopyFrom(*item->GetItemInfo());
	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, Protocol::S_EQUIPPED_ITEM);
	GetRoomRef()->BroadCast(sendBuffer);


	//TODO 나중에는 DB 접근을 좀 더 효율적으로 수정하자.
//장착했다고 정보 수정해주기
	DBConnection* db = GDBConnectionPool->Pop();
	db->SaveItemInfo(item->GetItemInfo());
	GDBConnectionPool->Push(db);


	//아이템을 장비했으면 인벤토리를 갱신해주자.
	RefreshInven();
}

void Player::UseItem(shared_ptr<Item> item)
{
	//아이템을 사용했으니 인벤에서 지워주고,
	GetInven()->RemoveItem(item);

	if (item->GetItemType() == ItemType::CONSUMEABLE)
	{
		//지금은 포션 밖에 없으므로 HP 회복하게 해준다.
		int32 currentHp = GetHP();
		SetHP(currentHp + 50);

		Protocol::S_ChangedHP pkt;
		pkt.mutable_target()->CopyFrom(GetInfo());
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_CHANGEDHP);
		GetRoomRef()->BroadCast(sendBuffer);

		DBConnection* db = GDBConnectionPool->Pop();
		db->DeleteItemDb(item->GetItemDbId());
		GDBConnectionPool->Push(db);

	}

	RefreshInven();
}

void Player::Additem(shared_ptr<Item> item)
{
	//플레이어의 가방에 빈 슬롯이 없다면,
	int32 slot = GetInven()->GetEmptySlot();
	if (slot == -1)
	{
		cout << GetPlayerDb()->PlayerName << "의 가방이 꽉 찼습니다." << endl;
		return;
	}
	//먹은 아이템을 플레이어의 인벤에 넣어준다.
	item->GetItemInfo()->set_slot(slot);
	//플레이어가 아이템을 먹으면 DB에 데이터를 넣어준다..
	DBConnection* db = GDBConnectionPool->Pop();
	db->CreateItemDb(GetPlayerDb(), item->GetItemInfo());
	GDBConnectionPool->Push(db);

	GetInven()->AddItem(item);
}

void Player::BuyItem(int32 itemTemplatedId)
{
	Protocol::ItemInfo buyItem = GDataManager->GetItemData(itemTemplatedId);

	int32 playerGold = GetInven()->GetGold();
	if (playerGold < buyItem.price())
	{
		Protocol::S_BuyItem pkt;
		pkt.set_result(0);
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_BUY_ITEM);
		GetSession()->Send(sendBuffer);
		return;
	}
	GetInven()->AddGold(-buyItem.price());

	shared_ptr<Item> item = Item::MakeItem(&buyItem);
	Additem(item);
	RefreshInven();
}

void Player::AcceptQuest(int32 questTemplatedId)
{

	DBConnection* db = GDBConnectionPool->Pop();
	FQuestData data = GDataManager->GetQuestData(questTemplatedId);

	//이미 퀘스트가 있으면 거절한다.
	if (GetPlayerDb()->CurrentQuestList.contains(questTemplatedId) == true)
		return;

	GetPlayerDb()->AddQuest(questTemplatedId);
	shared_ptr<Player> player = static_pointer_cast<Player>(shared_from_this());
	db->CreateQuestDb(GetPlayerDb(), data);
	GDBConnectionPool->Push(db);

	Protocol::S_GetQuest pkt2;
	pkt2.set_currentprogress(0);
	pkt2.set_questtemplatedid(questTemplatedId);

	SendBufferRef sendBuffer =  ServerPacketHandler::MakeSendBuffer(pkt2, Protocol::S_GET_QUEST);
	GetSession()->Send(sendBuffer);

}

void Player::RefreshQuest()
{
	//아무런 퀘스트가 없다.
	if (GetPlayerDb()->CurrentQuestList.size() == 0)
		return;


	for (auto data : GetPlayerDb()->CurrentQuestList)
	{
		FQuestData& questData = data.second;

		Protocol::S_GetQuest pkt2;
		pkt2.set_currentprogress(questData.CurrentProgress);
		pkt2.set_questtemplatedid(questData.Id);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt2, Protocol::S_GET_QUEST);
		GetSession()->Send(sendBuffer);
	}
}

void Player::RefreshInven()
{
	Protocol::S_RefreshInventory pkt2;
	for (auto item : GetInven()->GetItemList())
	{
		Protocol::ItemInfo* addItem = pkt2.add_items();
		*addItem = *item.second->GetItemInfo();
	}

	pkt2.set_gold(GetGold());

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt2, S_REFRESH_INVENTORY);
	GetSession()->Send(sendBuffer);

	DBConnection* db = GDBConnectionPool->Pop();

	db->SavePlayerInfo(static_pointer_cast<Player>(shared_from_this()));
	GDBConnectionPool->Push(db);
}

void Player::CheckQuestTarget(int32 targetTemplatedId)
{
	//아무런 퀘스트가 없다.
	if (GetPlayerDb()->CurrentQuestList.size() == 0)
		return;


	for (auto& data : GetPlayerDb()->CurrentQuestList)
	{
		FQuestData& questData = data.second;
		if (questData.TargetTemplatedId != targetTemplatedId)
			continue;

		//사냥한 대상이 목표 대상이 맞다면,
		questData.CurrentProgress++;

		DBConnection* db = GDBConnectionPool->Pop();
		shared_ptr<Player> player = static_pointer_cast<Player>(shared_from_this());
		db->SaveQuestInfo(questData, player);
		GDBConnectionPool->Push(db);

		//플레이어한테 패킷을 보내주자.
		Protocol::S_UpdateQuest pkt;
		pkt.set_currentprogress(questData.CurrentProgress);
		pkt.set_questtemplatedid(questData.Id);
		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(pkt, S_QUEST_UPDATE);
		GetSession()->Send(sendBuffer);
	}
}

bool Player::OnDead(Protocol::ObjectInfo damageCauser)
{
	bool ret = Creature::OnDead(damageCauser);
	if (ret == false)
		return false;

	shared_ptr<Creature> causer = static_pointer_cast<Creature>(GetRoomRef()->FindObjectById(damageCauser.id()));
	causer->UnBindTarget();
	//다시 부활 시켜주자.

	shared_ptr<PlayerDb> playerDb = GetPlayerDb();
	playerDb->Hp = playerDb->MaxHp;
	playerDb->Pos.set_locationx(0);
	playerDb->Pos.set_locationy(0);
	playerDb->Pos.set_locationz(200);

	shared_ptr<Player> thisPlayer = static_pointer_cast<Player>(shared_from_this());
	GetRoomRef()->Revive(GetSession(), playerDb);
	//데이터 베이스에 저장.
	DBConnection* db = GDBConnectionPool->Pop();
	db->SavePlayerInfo(thisPlayer);
	GDBConnectionPool->Push(db);
	return true;

}

void Player::LoadPlayerDb()
{
	_info.set_name(_playerDb->PlayerName);
	
	Protocol::Stat* stat	= _info.mutable_stat();
	stat->set_level(_playerDb->Level);
	stat->set_hp(_playerDb->Hp);
	stat->set_maxhp(_playerDb->MaxHp);
	stat->set_damage(_playerDb->Attack);
	stat->set_totalexp(_playerDb->TotalExp);
	Protocol::Position* pos = _info.mutable_position();
	pos->CopyFrom(_playerDb->Pos);
	
	GetInven()->AddGold(_playerDb->Gold);
}

void Player::AddExp(int32 value)
{
	Protocol::Stat* currentStat = _info.mutable_stat();
	int32 currentExp = currentStat->totalexp();
	int32 nextExp = currentExp + value;
	currentStat->set_totalexp(nextExp);
}

void Player::Disconnect()
{
	Protocol::ObjectInfo info = GetInfo();
	GetRoomRef()->Remove(GetInfo());
	GetSession()->Send(ServerPacketHandler::Make_S_Disconnect(info));

	//연결이 끊어지면 데이터 베이스에 저장.
	DBConnection* db = GDBConnectionPool->Pop();
	shared_ptr<Player> thisPlayer = static_pointer_cast<Player>(shared_from_this());
	db->SavePlayerInfo(thisPlayer);
	GDBConnectionPool->Push(db);

	printf("ID[%d], Name[%s]의 연결이 끊어졌습니다.", info.id(), info.name().c_str());
}

void Player::SetUsingWeapon(Protocol::ItemInfo* weapon)
{
	_usingWeapon = make_shared<Protocol::ItemInfo>();
	_usingWeapon->CopyFrom(*weapon);
	SetDamage(weapon->damage());
}

