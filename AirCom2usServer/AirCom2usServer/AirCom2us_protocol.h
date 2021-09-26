#pragma once

#define SERVER_PORT		49152
constexpr int MAX_USER = 10000;
constexpr int MAX_PLAYER_IDX = 10000;
constexpr int MAX_PLANE1_IDX = 30000;
constexpr int MAX_PLANE2_IDX = 50000;
constexpr int MAX_PLANE3_IDX = 70000;
constexpr int MAX_BOSS1_IDX = 85000;
constexpr int MAX_BOSS2_IDX = 100000;

constexpr int MAX_SESSION = 10000;

enum CS {
	LOGIN,
	MOVE,
	LOGOUT,
	CREATE_SESSION,
	SESSION_END,
};

enum SC {
	LOGIN_OK,
	LOGIN_FAIL,
	POSITION,
	SET_SESSION_OK,
	ADD_OBJECT,
	END_SESSION,
	REMOVE_OBJECT,
};
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
	int sessionId;
};

struct sc_packet_add_object {
	unsigned char size;
	unsigned char type;
	int id;
	int hp;
};

struct sc_packe_end_session {
	unsigned char size;
	unsigned char type;
};

struct sc_packet_remove_object {
	unsigned char size;
	unsigned char type;
	int id;
};

struct cs_packet_login {
	unsigned char	size;
	unsigned char	type;
};

struct cs_packet_move {
	unsigned char	size;
	unsigned char	type;
	float	x, y;
	float move_time;
	int id;
};

struct cs_packet_logout {
	unsigned char	size;
	unsigned char	type;
};

struct cs_packet_create_session {
	unsigned char	size;
	unsigned char	type;
	int sessionType;
	// 어떤 스테이지인지 확인할 수 있는 변수 추가 가능
};

struct cs_packet_cs_packet_session_endcreate_session {
	unsigned char	size;
	unsigned char	type;
};

#pragma pack (pop)