#pragma once

#include "CorePch.h"
#include "Protocol.pb.h"


using GameSessionRef = std::shared_ptr<class GameSession>;
using GameRoomRef = std::shared_ptr<class GameRoom>;
using GameObjectRef = std::shared_ptr<class GameObject>;
using PlayerRef = std::shared_ptr<class Player>;
using MonsterRef = std::shared_ptr<class Monster>;
using CreatureRef = std::shared_ptr<class Creature>;

using namespace Protocol;

extern class DBConnectionPool* GDBConnectionPool;

#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")

#ifdef _DEBUG
#pragma comment(lib, "ServerCore\\Debug\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "ServerCore\\Release\\ServerCore.lib")
#pragma comment(lib, "Protobuf\\Release\\libprotobuf.lib")
#endif

#include "ServerPacketHandler.h"