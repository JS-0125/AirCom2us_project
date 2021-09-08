#pragma once



#define SERVER_PORT		49152
constexpr int MAX_USER = 10000;
constexpr int MAX_SESSION = 10000;

#define CS_LOGIN		0		// 클라이언트가 서버에 접속 요청
#define CS_MOVE		1		// 클라이언트가 아바타기 이동을 서버에 요청
#define CS_LOGOUT	2		// 클라이언트 종료
#define CS_CREATE_SESSION	3		

#define SC_LOGIN_OK		0	// CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 수락
#define SC_LOGIN_FAIL		1	// CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 거절
#define SC_POSITION		2	// OBJECT의 위치 변경을 클라이언트에 통보
#define SC_SET_SESSION_OK	3		


#pragma pack(push ,1)

struct sc_packet_login_ok {
	unsigned char size;
	unsigned char type;
	int id;
	int	x, y;
	int LEVEL, EXP;
};

struct sc_packet_login_fail {
	unsigned char size;
	unsigned char type;
};

struct sc_packet_position {
	unsigned char size;
	unsigned char type;
	int id;
	float x, y;
	int move_time;			// Stress Test 프로그램에서 delay를 측정할 때 사용, 
					// 서버는 해당 id가 접속한 클라이언트에서 보내온 최신 값을 return 해야 한다.
};

struct sc_packet_set_session_ok {
	unsigned char size;
	unsigned char type;
};

struct cs_packet_login {
	unsigned char	size;
	unsigned char	type;
};

struct cs_packet_move {
	unsigned char	size;
	unsigned char	type;
	float	x, y;
	int move_time;
};

struct cs_packet_logout {
	unsigned char	size;
	unsigned char	type;
};

struct cs_packet_create_session {
	unsigned char	size;
	unsigned char	type;
	// 어떤 스테이지인지 확인할 수 있는 변수 추가 가능
};

#pragma pack (pop)