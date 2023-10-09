#pragma once
#include <sql.h>
#include "DataContents.h"
#include <sqlext.h>
#pragma warning(disable : 4996)

/*----------------
	DBConnection
-----------------*/

class PlayerDb;
class ItemDb;
class Account;
class PlayerDb;

enum
{
	WVARCHAR_MAX = 4000,
	BINARY_MAX = 8000
};

class DBConnection
{
public:
	bool	Connect(SQLHENV henv, const WCHAR* connectionString);
	void	Clear();

	bool	Execute(const WCHAR* query);
	bool	Fetch();
	int32	GetRowCount();
	void	Unbind();

	bool	MakeAccount(string playerId, string password);
	int32	CheckAccount(string playerId, string password);
	bool	CheckAccount(string playerId);

	void CreatePlayerDb(shared_ptr<Account> account, string name = "", int32 templatedId = 1);
	void CreateItemDb(shared_ptr<PlayerDb> player, Protocol::ItemInfo* info);
	void CreateQuestDb(shared_ptr<PlayerDb> player, FQuestData data);

	void DeletePlayerDb(int32 accountId, int32 playerDbId);
	void DeleteItemDb(int32 itemDbId);

	vector<shared_ptr<PlayerDb>> LoadPlayerDb(shared_ptr<Account> account);
	vector<shared_ptr<ItemDb>> LoadItemDb(shared_ptr<PlayerDb> player);
	map<int32, FQuestData> LoadQuestDb(shared_ptr<PlayerDb> player);

	void SavePlayerInfo(shared_ptr<Player> player);
	void SaveItemInfo(Protocol::ItemInfo* info);
	void SaveQuestInfo(FQuestData data, shared_ptr<Player> player);

	string w2s(const std::wstring& var);
	wstring s2w(const std::string& var);

public:
	bool			BindParam(int32 paramIndex, bool* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, float* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, double* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int8* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int16* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int32* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, int64* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool			BindParam(int32 paramIndex, const WCHAR* str, SQLLEN* index);
	bool			BindParam(int32 paramIndex, const BYTE* bin, int32 size, SQLLEN* index);

	bool			BindCol(int32 columnIndex, bool* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, float* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, double* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int8* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int16* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int32* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, int64* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, TIMESTAMP_STRUCT* value, SQLLEN* index);
	bool			BindCol(int32 columnIndex, WCHAR* str, int32 size, SQLLEN* index);
	bool			BindCol(int32 columnIndex, BYTE* bin, int32 size, SQLLEN* index);

private:
	bool			BindParam(SQLUSMALLINT paramIndex, SQLSMALLINT cType, SQLSMALLINT sqlType, SQLULEN len, SQLPOINTER ptr, SQLLEN* index);
	bool			BindCol(SQLUSMALLINT columnIndex, SQLSMALLINT cType, SQLULEN len, SQLPOINTER value, SQLLEN* index);
	void			HandleError(SQLRETURN ret);

private:
	SQLHDBC			_connection = SQL_NULL_HANDLE;
	SQLHSTMT		_statement = SQL_NULL_HANDLE;
};



/*
샘플 코드


	//// Create Table
	//{
	//	auto query = L"									\
	//		DROP TABLE IF EXISTS [PlayerAccount];			\
	//		CREATE TABLE [PlayerAccount]					\
	//		(											\
	//			[id] INT NOT NULL PRIMARY KEY IDENTITY, \
	//			[PlayerId] NVARCHAR(50) NULL,	\
	//			[Password] NVARCHAR(50) NULL							\
	//		);";

	//	DBConnection* dbConn = GDBConnectionPool->Pop();
	//	ASSERT_CRASH(dbConn->Execute(query));
	//	GDBConnectionPool->Push(dbConn);
	//}

	//// Add Data
	//for (int32 i = 0; i < 3; i++)
	//{
	//	DBConnection* dbConn = GDBConnectionPool->Pop();
	//	dbConn->MakeAccount("Test", "1234가");
	//	//// 기존에 바인딩 된 정보 날림
	//	//dbConn->Unbind();

	//	//// 넘길 인자 바인딩

	//	//WCHAR playerId[100] = L"Test";
	//	//WCHAR password[100] = L"!234";
	//	//SQLLEN IdLen = 0;
	//	//SQLLEN passwordlen = 0;

	//	//// 넘길 인자 바인딩
	//	//ASSERT_CRASH(dbConn->BindParam(1, playerId, &IdLen));
	//	//ASSERT_CRASH(dbConn->BindParam(2, password, &passwordlen));

	//	//// SQL 실행
	//	//ASSERT_CRASH(dbConn->Execute(L"INSERT INTO [PlayerAccount]([PlayerId], [Password]) VALUES(?, ?)"));

	//	GDBConnectionPool->Push(dbConn);
	//}

	// Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		// 기존에 바인딩 된 정보 날림
		dbConn->Unbind();

		WCHAR outId[100];
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, outId, sizeof(outId)/ sizeof(WCHAR),& outIdLen));

		WCHAR outPassword[100];
		SQLLEN outPasswordLen = 0;
		ASSERT_CRASH(dbConn->BindCol(2, outPassword, sizeof(outPassword)/ sizeof(WCHAR), &outPasswordLen));

		// SQL 실행
		ASSERT_CRASH(dbConn->Execute(L"SELECT PlayerId, Password FROM [PlayerAccount]"));

		while (dbConn->Fetch())
		{
			wcout << "Id: " << outId << " Password : " << outPassword << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}

	// Read
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		// 기존에 바인딩 된 정보 날림
		dbConn->Unbind();

		int32 gold = 100;
		SQLLEN len = 0;
		// 넘길 인자 바인딩
		ASSERT_CRASH(dbConn->BindParam(1, &gold, &len));

		int32 outId = 0;
		SQLLEN outIdLen = 0;
		ASSERT_CRASH(dbConn->BindCol(1, &outId, &outIdLen));

		int32 outGold = 0;
		SQLLEN outGoldLen = 0;
		ASSERT_CRASH(dbConn->BindCol(2, &outGold, &outGoldLen));

		// SQL 실행
		ASSERT_CRASH(dbConn->Execute(L"SELECT id, gold FROM [dbo].[Gold] WHERE gold = (?)"));

		while (dbConn->Fetch())
		{
			cout << "Id: " << outId << " Gold : " << outGold << endl;
		}

		GDBConnectionPool->Push(dbConn);
	}

*/