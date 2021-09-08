using System.Runtime.InteropServices;

//public enum SERVER_PORT 3500;


public enum CS
{
    LOGIN,  // 클라이언트가 서버에 접속 요청
    MOVE,   // 클라이언트가 아바타기 이동을 서버에 요청
    LOGOUT,     // 클라이언트 종료
	CREATE_SESSION,
}

public enum SC
{
    LOGIN_OK,// CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 수락
    LOGIN_FAIL,  // CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 거절
    POSITION,   // OBJECT의 위치 변경을 클라이언트에 통보
	SET_SESSION_OK,
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
public class sc_packet_login_ok
{
	public byte size;
	public byte type;
	public int id;
	public int x, y;
	public int level, exp;

	public sc_packet_login_ok() { }
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class sc_packet_login_fail
{
	byte size;
	char type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class sc_packet_position
{
	public byte size;
	public byte type;
	public int id;
	public float x, y;
	public int move_time;          // Stress Test 프로그램에서 delay를 측정할 때 사용, 
							// 서버는 해당 id가 접속한 클라이언트에서 보내온 최신 값을 return 해야 한다.
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class sc_packet_set_session_ok
{
	public byte size;
	public byte type;
};


[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_login
{
	byte size;
	byte type;
	public cs_packet_login(byte packet_size, byte packet_type)
	{
		size = packet_size;
		type = packet_type;
	}
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_move
{
	public byte size;
	public byte type;
	public float x, y;
	public int move_time;
	public cs_packet_move(){}
	public cs_packet_move (byte packet_size, byte packet_type, float posX, float posY, int current_time)
    {
		size = packet_size;
		type = packet_type;
		x = posX;
		y = posY;
		move_time = current_time;
    }
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_logout
{
	byte size;
	char type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_create_session
{
	byte size;
	byte type;
	// 어떤 스테이지인지 확인할 수 있는 변수 추가 가능

	public cs_packet_create_session(byte packet_size, byte packet_type)
	{
		size = packet_size;
		type = packet_type;
	}
};