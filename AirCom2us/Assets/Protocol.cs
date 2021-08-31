using System.Runtime.InteropServices;

//public enum SERVER_PORT 3500;


public enum CS
{
    LOGIN,  // 클라이언트가 서버에 접속 요청
    MOVE,   // 클라이언트가 아바타기 이동을 서버에 요청
    LOGOUT,		// 클라이언트 종료
}

public enum SC
{
    LOGIN_OK,// CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 수락
    LOGIN_FAIL,  // CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 거절
    POSITION,	// OBJECT의 위치 변경을 클라이언트에 통보
}


//[StructLayout(LayoutKind.Sequential, Pack = 1)]
//public class socket_data
//{
//    public int msg;
//    public short size;
//    public short type;
//    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 100)] public char[] data;
//}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct sc_packet_login_ok
{
	byte size;
	char type;
	int id;
	short x, y;
	int HP, LEVEL, EXP;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct sc_packet_login_fail
{
	byte size;
	char type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct sc_packet_position
{
	byte size;
	char type;
	int id;
	short x, y;
	int move_time;          // Stress Test 프로그램에서 delay를 측정할 때 사용, 
							// 서버는 해당 id가 접속한 클라이언트에서 보내온 최신 값을 return 해야 한다.
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct cs_packet_login
{
	byte size;
	char type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct cs_packet_move
{
	byte size;
	char type;
	char direction;     // 0:Up, 1:Down, 2:Left, 3:Right
	int move_time;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct cs_packet_logout
{
	byte size;
	char type;
};