#pragma once



#define SERVER_PORT		49152
constexpr int MAX_USER = 10000;
constexpr int MAX_SESSION = 10000;

#define CS_LOGIN		0		// Ŭ���̾�Ʈ�� ������ ���� ��û
#define CS_MOVE		1		// Ŭ���̾�Ʈ�� �ƹ�Ÿ�� �̵��� ������ ��û
#define CS_LOGOUT	2		// Ŭ���̾�Ʈ ����
#define CS_CREATE_SESSION	3		

#define SC_LOGIN_OK		0	// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
#define SC_LOGIN_FAIL		1	// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
#define SC_POSITION		2	// OBJECT�� ��ġ ������ Ŭ���̾�Ʈ�� �뺸
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
	int move_time;			// Stress Test ���α׷����� delay�� ������ �� ���, 
					// ������ �ش� id�� ������ Ŭ���̾�Ʈ���� ������ �ֽ� ���� return �ؾ� �Ѵ�.
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
	// � ������������ Ȯ���� �� �ִ� ���� �߰� ����
};

#pragma pack (pop)