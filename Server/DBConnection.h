#pragma once
#include <sql.h>
#include <sqlext.h>

/*----------------
	DBConnection
-----------------*/

enum
{
	WVARCHAR_MAX = 4000,
	BINARY_MAX = 8000
};

class DBConnection
{
public:
	bool			Connect(SQLHENV henv, const WCHAR* connectionString);
	void			Clear();

	bool			Execute(const WCHAR* query);
	bool			Fetch();
	int32			GetRowCount();
	void			Unbind();

	bool			MakeAccount(string playerId, string password);
	bool			CheckAccount(string playerId, string password);
	bool			CheckAccount(string playerId);

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