#include "pch.h"
#include "DBConnection.h"
#include "DataManager.h"
#include "Account.h"
#include <cstddef>
#include <locale>
#include "Player.h"

/*----------------
	DBConnection
-----------------*/

bool DBConnection::Connect(SQLHENV henv, const WCHAR* connectionString)
{
	if (::SQLAllocHandle(SQL_HANDLE_DBC, henv, &_connection) != SQL_SUCCESS)
		return false;

	WCHAR stringBuffer[MAX_PATH] = { 0 };
	::wcscpy_s(stringBuffer, connectionString);

	WCHAR resultString[MAX_PATH] = { 0 };
	SQLSMALLINT resultStringLen = 0;

	SQLRETURN ret = ::SQLDriverConnectW(
		_connection,
		NULL,
		reinterpret_cast<SQLWCHAR*>(stringBuffer),
		_countof(stringBuffer),
		OUT reinterpret_cast<SQLWCHAR*>(resultString),
		_countof(resultString),
		OUT & resultStringLen,
		SQL_DRIVER_NOPROMPT
	);

	if (::SQLAllocHandle(SQL_HANDLE_STMT, _connection, &_statement) != SQL_SUCCESS)
		return false;

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

void DBConnection::Clear()
{
	if (_connection != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_DBC, _connection);
		_connection = SQL_NULL_HANDLE;
	}

	if (_statement != SQL_NULL_HANDLE)
	{
		::SQLFreeHandle(SQL_HANDLE_STMT, _statement);
		_statement = SQL_NULL_HANDLE;
	}
}

bool DBConnection::Execute(const WCHAR* query)
{
	SQLRETURN ret = ::SQLExecDirectW(_statement, (SQLWCHAR*)query, SQL_NTSL);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return true;

	HandleError(ret);
	return false;
}

bool DBConnection::Fetch()
{
	SQLRETURN ret = ::SQLFetch(_statement);

	switch (ret)
	{
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		return true;
	case SQL_NO_DATA:
		return false;
	case SQL_ERROR:
		HandleError(ret);
		return false;
	default:
		return true;
	}
}

int32 DBConnection::GetRowCount()
{
	SQLLEN count = 0;
	SQLRETURN ret = ::SQLRowCount(_statement, OUT & count);

	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		return static_cast<int32>(count);

	return -1;
}

void DBConnection::Unbind()
{
	::SQLFreeStmt(_statement, SQL_UNBIND);
	::SQLFreeStmt(_statement, SQL_RESET_PARAMS);
	::SQLFreeStmt(_statement, SQL_CLOSE);
}

bool DBConnection::MakeAccount(string playerId, string password)
{
	Unbind();

	wstring wid;
	wstring wpassword;
	wid.assign(playerId.begin(), playerId.end());
	wpassword.assign(password.begin(), password.end());

	SQLLEN IdLen = 0;
	SQLLEN passwordlen = 0;

	//이미 존재하는 아이디 인지 확인
	bool ret = CheckAccount(playerId);
	if (ret == true)
	{
		wcout << wid <<"는 중복된 계정입니다." << endl;
		return false;
	}

	Unbind();
	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, wid.data(), &IdLen));
	ASSERT_CRASH(BindParam(2, wpassword.data(), &passwordlen));

	// SQL 실행
	ASSERT_CRASH(Execute(L"INSERT INTO [PlayerAccount]([PlayerId], [Password]) VALUES(?, ?)"));

	wcout << wid << " 새로운 계정 추가." << endl;



	return true;
}


//계정이 이미 존재하는지 확인용
bool DBConnection::CheckAccount(string playerId)
{
	wstring wid;
	wid.assign(playerId.begin(), playerId.end());


	//아이디와 패스워드를 확인해본다.
	WCHAR outId[100];
	SQLLEN outIdLen = 0;
	ASSERT_CRASH(BindCol(1, outId, sizeof(outId) / sizeof(WCHAR), &outIdLen));
	// SQL 실행
	ASSERT_CRASH(Execute(L"SELECT [PlayerId] FROM [PlayerAccount]"));

	while (Fetch())
	{
		if (wcscmp(wid.data(), outId) == 0)
		{
			//테이블 안의 정보들 중에 플레이어가 입력한 아이디와 비밀번호가 있다면 통과.
			return true;
		}
	}
	return false;
}


int32 DBConnection::CheckAccount(string playerId, string password)
{
	Unbind();

	wstring wid;
	wstring wpassword;
	wid.assign(playerId.begin(), playerId.end());
	wpassword.assign(password.begin(), password.end());


	//아이디와 패스워드를 확인해본다.
	WCHAR outId[100];
	SQLLEN outIdLen = 0;
	ASSERT_CRASH(BindCol(1, outId, sizeof(outId)/ sizeof(WCHAR),& outIdLen));

	WCHAR outPassword[100];
	SQLLEN outPasswordLen = 0;
	ASSERT_CRASH(BindCol(2, outPassword, sizeof(outPassword)/ sizeof(WCHAR), &outPasswordLen));

	int32 DbId;;
	SQLLEN outDbLen = 0;
	ASSERT_CRASH(BindCol(3, &DbId, &outDbLen));
	//bool DBConnection::BindCol(int32 columnIndex, int32 * value, SQLLEN * index)
	//{
	//	return BindCol(columnIndex, SQL_C_LONG, size32(int32), value, index);
	//}

	// SQL 실행
	ASSERT_CRASH(Execute(L"SELECT [PlayerId], [Password], [id] FROM [PlayerAccount]"));

	while (Fetch())
	{
		if (wcscmp(wid.data(), outId) == 0 && wcscmp(wpassword.data(), outPassword) == 0 )
		{
			//테이블 안의 정보들 중에 플레이어가 입력한 아이디와 비밀번호가 있다면 통과.
			return DbId;
		}
	}
	return 0;
}

vector<shared_ptr<class PlayerDb>> DBConnection::LoadPlayerDb(shared_ptr<Account> account)
{
	Unbind();

	int32 id = 0;
	SQLLEN idLen = 0;

	WCHAR name[100];
	SQLLEN nameLen = 0;

	int32 level = 0;
	SQLLEN levelLen = 0;

	int32 hp = 0;
	SQLLEN hpLen = 0;

	int32 maxHp = 0;
	SQLLEN maxHpLen = 0;

	int32 gold = 0;
	SQLLEN goldLen = 0;

	int32 totalExp = 0;
	SQLLEN expLen = 0;

	int32 posX = 0;
	SQLLEN xLen = 0;

	int32 posY = 0;
	SQLLEN yLen = 0;

	int32 posZ = 0;
	SQLLEN zLen = 0;

	int32 templatedId = 0;
	SQLLEN temLen = 0;

	int32 playerSlot = 0;
	SQLLEN slotLen = 0;

	ASSERT_CRASH(BindCol(1, &id, &idLen));
	ASSERT_CRASH(BindCol(2, name, sizeof(name) / sizeof(WCHAR), &nameLen));
	ASSERT_CRASH(BindCol(3, &level, &levelLen));
	ASSERT_CRASH(BindCol(4, &hp, &hpLen));
	ASSERT_CRASH(BindCol(5, &maxHp, &maxHpLen));
	ASSERT_CRASH(BindCol(6, &gold, &goldLen));
	ASSERT_CRASH(BindCol(7, &totalExp, &expLen));
	ASSERT_CRASH(BindCol(8, &posX, &xLen));
	ASSERT_CRASH(BindCol(9, &posY, &yLen));
	ASSERT_CRASH(BindCol(10, &posZ, &zLen));
	ASSERT_CRASH(BindCol(11, &templatedId, &temLen));
	ASSERT_CRASH(BindCol(12, &playerSlot, &slotLen));

	int32 accountId = account->GetAccountDbId();
	SQLLEN len = 0;
	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &accountId, &len));

	// SQL 실행
	ASSERT_CRASH(Execute(L"SELECT [PlayerDbId], [PlayerName],  [Level], [Hp], [MaxHp], [Gold], [TotalExp], [posX], [PosY], [PosZ], [TemplatedId], [PlayerSlot] FROM [Player] WHERE [AccountDbId]= (?)"));
	vector<shared_ptr<class PlayerDb>> players;
	while (Fetch())
	{
		shared_ptr<PlayerDb> player = make_shared<PlayerDb>();
		player->PlayerDbId = id;
		player->PlayerName = w2s(name);
		player->Level = level;
		player->Hp = hp;
		player->Attack = 10;
		player->MaxHp = maxHp;
		player->Gold = gold;
		player->TotalExp = totalExp;
		player->AccountDbId = accountId;
		player->AccountDb = account;
		player->Pos.set_locationx(posX);
		player->Pos.set_locationy(posY);
		player->Pos.set_locationz(posZ);
		player->TemplatedId=templatedId;
		player->PlayerSlot = playerSlot;
		players.push_back(player);

	}

	for (auto player : players)
	{

		player->Items = LoadItemDb(player);

	}

	for (auto player : players)
	{

		player->CurrentQuestList = LoadQuestDb(player);

	}

	return players;
}

vector<shared_ptr<ItemDb>> DBConnection::LoadItemDb(shared_ptr<PlayerDb> player)
{
	Unbind();

	int32 id = 0;
	SQLLEN idLen = 0;

	int32 templatedId = 0;
	SQLLEN templatedIdLen = 0;

	int32 count = 0;
	SQLLEN countLen = 0;

	int32 slot = 0;
	SQLLEN slotLen = 0;

	bool equipped = false;
	SQLLEN equippedLen = 0;

	ASSERT_CRASH(BindCol(1, &id, &idLen));
	ASSERT_CRASH(BindCol(2, &templatedId, &templatedIdLen));
	ASSERT_CRASH(BindCol(3, &count, &countLen));
	ASSERT_CRASH(BindCol(4, &slot, &slotLen));
	ASSERT_CRASH(BindCol(5, &equipped, &equippedLen));

	int32 playerId = player->PlayerDbId;
	SQLLEN len = 0;
	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &playerId, &len));

	// SQL 실행
	ASSERT_CRASH(Execute(L"SELECT [ItemDbId], [TemplatedId],  [Count], [Slot], [IsEquipped] FROM [Item] WHERE [OwnerPlayerId]= (?)"));
	vector<shared_ptr<ItemDb>> items;;
	while (Fetch())
	{
		shared_ptr<ItemDb> item = make_shared<ItemDb>();
		item->ItemDbId = id;
		item->TemplatedId = templatedId;
		item->Count = count;
		item->Slot = slot;
		item->IsEquipped = equipped;
		item->OwnerDbId = playerId;
		item->Owner = player;
	
		items.push_back(item);
	}
	return items;
}

map<int32, FQuestData> DBConnection::LoadQuestDb(shared_ptr<PlayerDb> player)
{
	map<int32, FQuestData> playerQuestList;

	Unbind();

	int32 id = 0;
	SQLLEN idLen = 0;

	int32 currentProgress = 0;
	SQLLEN currentLen = 0;

	ASSERT_CRASH(BindCol(1, &id, &idLen));
	ASSERT_CRASH(BindCol(2, &currentProgress, &currentLen));

	int32 playerId = player->PlayerDbId;
	SQLLEN len = 0;
	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &playerId, &len));

	// SQL 실행
	ASSERT_CRASH(Execute(L"SELECT [QuestId], [CurrentProgress] FROM [Quest] WHERE [PlayerId]= (?)"));
	while (Fetch())
	{
		int32 questId = id;
		FQuestData data = GDataManager->GetQuestData(id);
		data.CurrentProgress = currentProgress;
		playerQuestList[questId] = data;
	}
	return playerQuestList;
}

void DBConnection::SavePlayerInfo(shared_ptr<Player> player)
{
	Unbind();

	int32 level = player->GetLevel();
	SQLLEN levelLen = 0;

	int32 hp = player->GetHP();
	SQLLEN hpLen = 0;

	int32 maxHp = player->GetMaxHP();;
	SQLLEN maxHpLen = 0;

	int32 gold = player->GetGold();;
	SQLLEN goldLen = 0;

	int32 exp = player->GetExp();;
	SQLLEN expLen = 0;

	int32 posX = player->GetPos().locationx();
	SQLLEN xLen = 0;

	int32 posY = player->GetPos().locationy();
	SQLLEN yLen = 0;

	int32 posZ = player->GetPos().locationz();
	SQLLEN zLen = 0;

	//플레이어 DB ID
	int32 id = player->GetPlayerDbId();
	SQLLEN idLen = 0;



	ASSERT_CRASH(BindParam(1, &level, &levelLen));
	ASSERT_CRASH(BindParam(2, &hp, &hpLen));
	ASSERT_CRASH(BindParam(3, &maxHp, &maxHpLen));
	ASSERT_CRASH(BindParam(4, &gold, &goldLen));
	ASSERT_CRASH(BindParam(5, &exp, &expLen));
	ASSERT_CRASH(BindParam(6, &posX, &xLen));
	ASSERT_CRASH(BindParam(7, &posY, &yLen));
	ASSERT_CRASH(BindParam(8, &posZ, &zLen));
	ASSERT_CRASH(BindParam(9, &id, &idLen));

	// SQL 실행
	ASSERT_CRASH(Execute(L"UPDATE [Player] SET [Level] = (?), [Hp] = (?), [MaxHp] = (?), [Gold] = (?), [TotalExp] = (?), [PosX] = (?), [PosY] = (?), [PosZ] = (?) WHERE [PlayerDbId]= (?)"));

	for (auto item : player->GetInven()->GetItemList())
	{
		SaveItemInfo(item.second->GetItemInfo());
	}

	for (auto quest : player->GetPlayerDb()->CurrentQuestList)
	{
		FQuestData data = quest.second;
		SaveQuestInfo(data, player);
	}

}

void DBConnection::SaveItemInfo(Protocol::ItemInfo* info)
{
	Unbind();

	int32 count = info->count();
	SQLLEN countLen = 0;

	int32 slot = info->slot();
	SQLLEN slotLen = 0;

	bool equipped = info->isequipped();
	SQLLEN eqLen = 0;

	int32 itemId = info->databaseid();
	SQLLEN idLen = 0;

	ASSERT_CRASH(BindParam(1, &count, &countLen));
	ASSERT_CRASH(BindParam(2, &slot, &slotLen));
	ASSERT_CRASH(BindParam(3, &equipped, &eqLen));
	ASSERT_CRASH(BindParam(4, &itemId, &idLen));


	// SQL 실행
	ASSERT_CRASH(Execute(L"UPDATE [Item] SET [Count] = (?), [Slot] = (?), [IsEquipped] = (?) WHERE [ItemDbId]= (?)"));

}

void DBConnection::SaveQuestInfo(FQuestData data, shared_ptr<Player> player)
{
	Unbind();

	int32 id = data.Id;
	SQLLEN idLen = 0;

	int32 currentProgress = data.CurrentProgress;
	SQLLEN currentLen = 0;

	int32 playerDbId = player->GetPlayerDbId();
	SQLLEN playerLen = 0;

	ASSERT_CRASH(BindParam(1, &id, &idLen));
	ASSERT_CRASH(BindParam(2, &currentProgress, &currentLen));
	ASSERT_CRASH(BindParam(3, &playerDbId, &playerLen));


	// SQL 실행
	ASSERT_CRASH(Execute(L"UPDATE [Quest] SET [QuestId] = (?), [CurrentProgress] = (?) WHERE [PlayerId]= (?)"));
}

void DBConnection::CreatePlayerDb(shared_ptr<Account> account, string name, int32 templatedId)
{
	Unbind();
	int32 Id = account->GetAccountDbId();
	SQLLEN Len = 0;
	ASSERT_CRASH(BindParam(1, &Id, &Len));

	//계정의 플레이어 슬롯 증가.
	ASSERT_CRASH(Execute(L"UPDATE [PlayerAccount] SET [UsingPlayerSlot] = [UsingPlayerSlot] + 1 WHERE [id] = (?)"));

	Unbind();

	wstring playerName;
	if (name == "")
		playerName = s2w(account->GetAccountName());
	else
		playerName = s2w(name);
	SQLLEN playerNameLen = 0;

	int32 slot = account->_players.size();
	SQLLEN slotLen = 0;

	int32 templated = templatedId;
	SQLLEN temLen = 0;

	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &Id, &Len));
	ASSERT_CRASH(BindParam(2, playerName.data(), &playerNameLen));
	ASSERT_CRASH(BindParam(3, &slot, &slotLen));
	ASSERT_CRASH(BindParam(4, &templated, &temLen));

	// SQL 실행
	ASSERT_CRASH(Execute(L"INSERT INTO [Player]([AccountDbId], [PlayerName], [PlayerSlot], [TemplatedId]) VALUES(?, ?, ?, ?)"));

	wcout << "[DBConnection] " << playerName << " 새로운 플레이어 추가." << endl;
}

void DBConnection::CreateItemDb(shared_ptr<PlayerDb> player, Protocol::ItemInfo* info)
{
	Unbind();


	//아이디와 패스워드를 확인해본다.
	int32 Id = player->PlayerDbId;
	SQLLEN Len = 0;

	int32 templatedId = info->templateid();
	SQLLEN templatedLen = 0;

	int32 slot = info->slot();
	SQLLEN slotLen = 0;

	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &Id, &Len));
	ASSERT_CRASH(BindParam(2, &templatedId, &templatedLen));
	ASSERT_CRASH(BindParam(3, &slot, &slotLen));

	// SQL 실행
	ASSERT_CRASH(Execute(L"INSERT INTO [Item]([OwnerPlayerId], [TemplatedId], [Slot]) VALUES(?, ?, ?)"));

	// 데이터베이스에서 자동으로 생성된 Id 값을 가져오기
	SQLRETURN retcode;
	SQLINTEGER insertedId;

	ASSERT_CRASH(Execute(L"SELECT @@IDENTITY"));
	Fetch();
	SQLGetData(_statement, 1, SQL_C_ULONG, &insertedId, sizeof(insertedId), NULL);

	info->set_databaseid(insertedId);

	// 변경 사항을 커밋
}

void DBConnection::CreateQuestDb(shared_ptr<PlayerDb> player, FQuestData data)
{
	Unbind();


	//아이디와 패스워드를 확인해본다.
	int32 playerId = player->PlayerDbId;
	SQLLEN Len = 0;

	int32 templatedId = data.Id;
	SQLLEN templatedLen = 0;

	int32 targetNumbers = 0;
	SQLLEN targetLen = 0;

	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &playerId, &Len));
	ASSERT_CRASH(BindParam(2, &templatedId, &templatedLen));
	ASSERT_CRASH(BindParam(3, &targetNumbers, &targetLen));

	// SQL 실행
	ASSERT_CRASH(Execute(L"INSERT INTO [Quest]([PlayerId], [QuestId], [CurrentProgress]) VALUES(?, ?, ?)"));
}

void DBConnection::DeletePlayerDb(int32 accountId, int32 playerDbId)
{
	Unbind();

	//아이디와 패스워드를 확인해본다.
	int32 Id = playerDbId;
	SQLLEN Len = 0;

	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &Id, &Len));

	// SQL 실행
	ASSERT_CRASH(Execute(L"DELETE FROM [Player] WHERE [PlayerDbId] = (?)"));

	Unbind();
	int32 accountdbId = accountId;
	SQLLEN accountLen = 0;
	ASSERT_CRASH(BindParam(1, &accountdbId, &accountLen));

	ASSERT_CRASH(Execute(L"UPDATE [PlayerAccount] SET [UsingPlayerSlot] = [UsingPlayerSlot] - 1 WHERE [id] = (?)"));
}

void DBConnection::DeleteItemDb(int32 itemDbId)
{
	Unbind();

	//소유한 플레이어 아이디 확인
	int32 Id = itemDbId;
	SQLLEN Len = 0;

	// 넘길 인자 바인딩
	ASSERT_CRASH(BindParam(1, &Id, &Len));

	// SQL 실행
	ASSERT_CRASH(Execute(L"DELETE FROM [Item] WHERE [itemDbId] = (?)"));
}


string DBConnection::w2s(const std::wstring& var)
{
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).to_bytes(var);
}

wstring DBConnection::s2w(const std::string& var)
{
	static std::locale loc("");
	auto& facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);
	return std::wstring_convert<std::remove_reference<decltype(facet)>::type, wchar_t>(&facet).from_bytes(var);
}

bool DBConnection::BindParam(int32 paramIndex, bool* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(bool), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, float* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_FLOAT, SQL_REAL, 0, value, index);
}

bool DBConnection::BindParam(int32 paramIndex, double* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_DOUBLE, SQL_DOUBLE, 0, value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int8* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_TINYINT, SQL_TINYINT, size32(int8), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int16* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_SHORT, SQL_SMALLINT, size32(int16), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int32* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_LONG, SQL_INTEGER, size32(int32), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, int64* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_SBIGINT, SQL_BIGINT, size32(int64), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindParam(paramIndex, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindParam(int32 paramIndex, const WCHAR* str, SQLLEN* index)
{
	SQLULEN size = static_cast<SQLULEN>((::wcslen(str) + 1) * 2);
	*index = SQL_NTSL;

	if (size > WVARCHAR_MAX)
		return BindParam(paramIndex, SQL_C_WCHAR, SQL_WLONGVARCHAR, size, (SQLPOINTER)str, index);
	else
		return BindParam(paramIndex, SQL_C_WCHAR, SQL_WVARCHAR, size, (SQLPOINTER)str, index);
}

bool DBConnection::BindParam(int32 paramIndex, const BYTE* bin, int32 size, SQLLEN* index)
{
	if (bin == nullptr)
	{
		*index = SQL_NULL_DATA;
		size = 1;
	}
	else
		*index = size;

	if (size > BINARY_MAX)
		return BindParam(paramIndex, SQL_C_BINARY, SQL_LONGVARBINARY, size, (BYTE*)bin, index);
	else
		return BindParam(paramIndex, SQL_C_BINARY, SQL_BINARY, size, (BYTE*)bin, index);
}

bool DBConnection::BindCol(int32 columnIndex, bool* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TINYINT, size32(bool), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, float* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_FLOAT, size32(float), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, double* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_DOUBLE, size32(double), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int8* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TINYINT, size32(int8), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int16* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_SHORT, size32(int16), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int32* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_LONG, size32(int32), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, int64* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_SBIGINT, size32(int64), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_TYPE_TIMESTAMP, size32(TIMESTAMP_STRUCT), value, index);
}

bool DBConnection::BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_C_WCHAR, size, str, index);
}

bool DBConnection::BindCol(int32 columnIndex, BYTE* bin, int32 size, SQLLEN* index)
{
	return BindCol(columnIndex, SQL_BINARY, size, bin, index);
}

bool DBConnection::BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index)
{
	SQLRETURN ret = ::SQLBindParameter(_statement, paramIndex, SQL_PARAM_INPUT, cType, sqlType, len, 0, ptr, 0, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

bool DBConnection::BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index)
{
	SQLRETURN ret = ::SQLBindCol(_statement, columnIndex, cType, value, len, index);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		HandleError(ret);
		return false;
	}

	return true;
}

void DBConnection::HandleError(SQLRETURN ret)
{
	if (ret == SQL_SUCCESS)
		return;

	SQLSMALLINT index = 1;
	SQLWCHAR sqlState[MAX_PATH] = { 0 };
	SQLINTEGER nativeErr = 0;
	SQLWCHAR errMsg[MAX_PATH] = { 0 };
	SQLSMALLINT msgLen = 0;
	SQLRETURN errorRet = 0;

	while (true)
	{
		errorRet = ::SQLGetDiagRecW(
			SQL_HANDLE_STMT,
			_statement,
			index,
			sqlState,
			OUT & nativeErr,
			errMsg,
			_countof(errMsg),
			OUT & msgLen
		);

		if (errorRet == SQL_NO_DATA)
			break;

		if (errorRet != SQL_SUCCESS && errorRet != SQL_SUCCESS_WITH_INFO)
			break;

		// TODO : Log
		wcout.imbue(locale("kor"));
		wcout << errMsg << endl;

		index++;
	}
}
