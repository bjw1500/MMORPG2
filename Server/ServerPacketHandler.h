#pragma once



using GameSessionRef = std::shared_ptr<class GameSession>;
class ServerPacketHandler
{
public:
	static void HandlePacket(GameSessionRef session, BYTE* buffer, int32 len);

	// Ingame
	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack);
	
	static SendBufferRef Make_S_EnterRoom();
	static SendBufferRef Make_S_SpawnMyPlayer(Protocol::ObjectInfo info);
	static SendBufferRef Make_S_Disconnect(Protocol::ObjectInfo info);

	static void Handle_C_EnterRoom(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_Move(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_Skill(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_ChangedHP(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_Chat(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_PickUpItem(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_MonsterMoveByClient(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_EquippedItem(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_UseItem(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_BuyItem(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_SpawnBoss(GameSessionRef& session, BYTE* buffer, int32 len);
	static void Handle_C_GetQuest(GameSessionRef& session, BYTE* buffer, int32 len);

	//pre - game
	static SendBufferRef Make_S_Connect();

	static void Handle_C_TryLogin(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_CreateAccount(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_LoadingCharacterList(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_CreateCharacter(GameSessionRef session, BYTE* buffer, int32 len);
	static void Handle_C_DeleteCharacter(GameSessionRef session, BYTE* buffer, int32 len);

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, uint16 pktId)
	{
		const uint16 dataSize = static_cast<uint16>(pkt.ByteSizeLong());
		const uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = make_shared<SendBuffer>(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		assert(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};