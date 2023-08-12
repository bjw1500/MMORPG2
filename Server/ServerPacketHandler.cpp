#include "pch.h"
#include <format>
#include "ServerPacketHandler.h"
#include "BufferReader.h"
#include "BufferWriter.h"
#include "Player.h"
#include "GameRoom.h"
#include "GameSession.h"
#include "DBConnection.h"
#include "DBConnectionPool.h"
#include "Account.h"


void ServerPacketHandler::HandlePacket(GameSessionRef session, BYTE* buffer, int32 len)
{
	PacketHeader* header = (PacketHeader*)buffer;
	//	uint16 id = header->id;
	uint16 size = header->size;

	switch (header->id)
	{
	case Protocol::C_MOVE:
		Handle_C_Move(session, buffer, len);
		break;
	case Protocol::C_SKILL:
		Handle_C_Skill(session, buffer, len);
		break;
	case Protocol::C_CHANGEDHP:
		Handle_C_ChangedHP(session, buffer, len);
		break;
	case Protocol::C_TRYLOGIN:
		Handle_C_TryLogin(session, buffer, len);
		break;
	case Protocol::C_ENTER_ROOM:
		Handle_C_EnterRoom(session, buffer, len);
		break;
	case Protocol::C_CREATEACCOUNT:
		Handle_C_CreateAccount(session, buffer, len);
		break;
	case Protocol::C_CHAT:
		Handle_C_Chat(session, buffer, len);
		break;
	case Protocol::C_PICKUPITEM:
		Handle_C_PickUpItem(session, buffer, len);
		break;
	default:
		break;
	}

}

SendBufferRef ServerPacketHandler::Make_S_TEST(uint64 id, uint32 hp, uint16 attack)
{
	Protocol::S_Test pkt;

	pkt.set_attack(attack);
	pkt.set_hp(hp);
	pkt.set_id(id);


	return MakeSendBuffer(pkt, Protocol::S_TEST);
}

SendBufferRef ServerPacketHandler::Make_S_Connect()
{
	Protocol::S_Connect pkt;
	pkt.set_info("서버 연결 성공");
	return MakeSendBuffer(pkt, Protocol::S_CONNECT);
}

SendBufferRef ServerPacketHandler::Make_S_EnterRoom()
{
	Protocol::S_EnterRoom pkt;

	return MakeSendBuffer(pkt, Protocol::S_ENTER_ROOM);
}

SendBufferRef ServerPacketHandler::Make_S_SpawnMyPlayer(Protocol::ObjectInfo info)
{
	S_SpawnMyPlayer pkt;
	ObjectInfo* objectInfo = pkt.mutable_info();
	*objectInfo = info;

	return MakeSendBuffer(pkt, S_SPAWN_MY_PLAYER);
}

SendBufferRef ServerPacketHandler::Make_S_Disconnect(Protocol::ObjectInfo info)
{
	Protocol::S_Disconnect pkt;
	Protocol::ObjectInfo* pktinfo = pkt.mutable_info();
	*pktinfo = info;

	return MakeSendBuffer(pkt, S_DISCONNECT);
}

void ServerPacketHandler::Handle_C_EnterRoom(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_EnterRoom pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	//TODO
	//나중에 플레이어가 요청한 캐릭터 ID를 생성하게 하자
	//하지만 지금은 그냥 기본 캐릭터로 입장만 시켜주자.
	GRoom->EnterPlayer(session);
}

void ServerPacketHandler::Handle_C_Move(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_Move pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	PlayerRef player = session->GetMyPlayer();
	if (player == nullptr)
	{
		printf("Handle C Move Error! [%s] 는 없는 플레이어 입니다.", pkt.info().name().c_str());
		return;
	}

	GameRoomRef room = player->GetRoomRef();
	if (room == nullptr)
	{
		printf("Handle C Move Error! [%s]의 방은 없는 방 입니다.", pkt.info().name().c_str());
		return;
	}

	room->HandleMove(pkt);
}

void ServerPacketHandler::Handle_C_Skill(GameSessionRef session, BYTE* buffer, int32 len)
{
	//플레이어가 스킬을 사용했다.
	//데미지 판정은 일단 클라이언트에게 맡기고, 사용했다는 패킷만 전부 뿌리게 하자.
	Protocol::C_Skill pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	PlayerRef player = session->GetMyPlayer();
	if (player == nullptr)
	{
		printf("Handle C Skill Error! [%s] 는 없는 플레이어 입니다.", pkt.info().name().c_str());
		return;
	}

	GameRoomRef room = player->GetRoomRef();
	if (room == nullptr)
	{
		printf("Handle C Skill Error! [%s]의 방은 없는 방 입니다.", pkt.info().name().c_str());
		return;
	}

	room->HandleSkill(pkt);
}

void ServerPacketHandler::Handle_C_ChangedHP(GameSessionRef session, BYTE* buffer, int32 len)
{
	//플레이어가 스킬을 사용했다.
//데미지 판정은 일단 클라이언트에게 맡기고, 사용했다는 패킷만 전부 뿌리게 하자.
	Protocol::C_ChangedHP pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	PlayerRef player = session->GetMyPlayer();
	if (player == nullptr)
	{
		printf("Handle C ChangedHP Error! [%s] 는 없는 플레이어 입니다.", pkt.target().name().c_str());
		return;
	}

	GameRoomRef room = player->GetRoomRef();
	if (room == nullptr)
	{
		printf("Handle C ChangedHP Error! [%s]의 방은 없는 방 입니다.", pkt.target().name().c_str());
		return;
	}

	room->HandleChangedHP(pkt);





}

void ServerPacketHandler::Handle_C_Chat(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_Chat pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	PlayerRef player = session->GetMyPlayer();
	if (player == nullptr)
	{
		return;
	}

	GameRoomRef room = player->GetRoomRef();
	if (room == nullptr)
	{
		return;
	}

	room->HandleChat(pkt);
}

void ServerPacketHandler::Handle_C_PickUpItem(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_PickUpItem pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	PlayerRef player = session->GetMyPlayer();
	if (player == nullptr)
	{
		return;
	}
	GameRoomRef room = player->GetRoomRef();
	if (room == nullptr)
	{
		return;
	}
	room->HandlePickUpItem(pkt);
}



//Pre - Game 
void ServerPacketHandler::Handle_C_TryLogin(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_TryLogin pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	cout << "ID[" << pkt.id() <<"]" << " 가 로그인을 시도합니다. 비밀번호" << pkt.password() << endl;


	//TODO
	//나중에 DB 연결해주기
	DBConnection* db = GDBConnectionPool->Pop();
	//db->MakeAccount(pkt.id(), pkt.password());

	//아이디와 비밀번화 확인 결과가 거짓이라면, 그냥 리턴.
	bool ret = db->CheckAccount(pkt.id(), pkt.password());
	if (ret == false)
	{
		Protocol::S_FailedLogin pkt3;
		pkt3.set_info("잘못된 계정입니다. 다시 입력해주세요.");
		session->Send(MakeSendBuffer(pkt3, Protocol::S_FAILEDLOGIN));
		GDBConnectionPool->Push(db);
		return;
	}
	GDBConnectionPool->Push(db);
	
	//세션에 플레이어의 계정 정보를 남겨두자.
	//게임 안에서 플레이어 닉넴으로 사용.
	session->SetMyAccount(make_shared<Account>(pkt.id()));


	//인증 절차, 이후 통과했으면 Success Login 패킷 보내주기
	
	
	Protocol::S_SuccessLogin pkt2;
	//string info = std::format("{1}", "안녕");
	std::string str = std::format("{}님 환영합니다.", pkt.id());
	pkt2.set_info(str);
	session->Send(MakeSendBuffer(pkt2, Protocol::S_SUCCESSLOGIN));


}

void ServerPacketHandler::Handle_C_CreateAccount(GameSessionRef session, BYTE* buffer, int32 len)
{
	Protocol::C_CreateAccount pkt;
	PacketHeader* header = (PacketHeader*)buffer;
	pkt.ParseFromArray(&header[1], header->size - sizeof(PacketHeader));

	cout << "ID[" << pkt.id() << "]" << " 가 새로운 계정을 만듭니다. 비밀번호" << pkt.password() << endl;


	//TODO
	//나중에 DB 연결해주기
	DBConnection* db = GDBConnectionPool->Pop();
	bool ret = db->MakeAccount(pkt.id(), pkt.password());


	Protocol::S_CreateAccount pkt2;
	pkt2.set_id(pkt.id());
	if (ret == false)
	{
		cout << pkt.id() <<" 새로운 계정 생성 실패." << endl;
		pkt2.set_result(false);
		pkt2.set_msg("이미 존재하는 계정입니다.");
	}
	else
	{
		cout << pkt.id() << " 새로운 계정 생성 성공." << endl;
		pkt2.set_result(true);
		string msg = pkt.id() + " 가 성공적으로 만들어졌습니다.";
		pkt2.set_msg(msg);
		session->SetMyAccount(make_shared<Account>(pkt.id()));
	}

	session->Send(MakeSendBuffer(pkt2, Protocol::S_CREATEACCOUNT));
	GDBConnectionPool->Push(db);
}
