#pragma once

#define SERVER_PORT		3500
constexpr int MAX_USER = 10000;

#define CS_LOGIN		1		// Ŭ���̾�Ʈ�� ������ ���� ��û
#define CS_MOVE		2		// Ŭ���̾�Ʈ�� �ƹ�Ÿ�� �̵��� ������ ��û
#define CS_LOGOUT	3		// Ŭ���̾�Ʈ ����

#define SC_LOGIN_OK		1	// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
#define SC_LOGIN_FAIL		2	// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
#define SC_POSITION		3	// OBJECT�� ��ġ ������ Ŭ���̾�Ʈ�� �뺸

#pragma pack(push ,1)

struct sc_packet_login_ok {
	unsigned char size;
	char type;
	int id;
	short	x, y;
	int	HP, LEVEL, EXP;
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
};

struct sc_packet_position {
	unsigned char size;
	char type;
	int id;
	short x, y;
	int move_time;			// Stress Test ���α׷����� delay�� ������ �� ���, 
					// ������ �ش� id�� ������ Ŭ���̾�Ʈ���� ������ �ֽ� ���� return �ؾ� �Ѵ�.
};

struct cs_packet_login {
	unsigned char	size;
	char	type;
};

struct cs_packet_move {
	unsigned char	size;
	char	type;
	char	direction;		// 0:Up, 1:Down, 2:Left, 3:Right
	int move_time;
};

struct cs_packet_logout {
	unsigned char	size;
	char	type;
};

#pragma pack (pop)