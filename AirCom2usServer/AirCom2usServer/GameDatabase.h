#pragma once
#include"default.h"
#include <windows.h>  
#include <string.h >
#include <atlstr.h>
#include"AirCom2us_protocol.h"

#define UNICODE  
#include <sqlext.h>  
using namespace std;

class GameDatabase
{
public:
	SQLHENV henv;
	SQLHDBC hdbc;
	SQLHSTMT hstmt = 0;

	GameDatabase();
	~GameDatabase();

	void HandleDiagnosticRecord(SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE RetCode);
	bool IdCheck(const char* id, sc_packet_login_ok& p);
	bool addID(const char* id, sc_packet_login_ok& p);
	bool updateChar(const char* id, sc_packet_login_ok& p);
};

