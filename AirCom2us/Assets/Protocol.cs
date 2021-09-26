using System.Runtime.InteropServices;

public enum OBJECT_ID_IDX
{
    MAX_USER = 100000,
    MAX_PLAYER_IDX = 10000,
    MAX_PLANE1_IDX = 30000,
    MAX_PLANE2_IDX = 50000,
    MAX_PLANE3_IDX = 70000,
    MAX_BOSS1_IDX = 85000,
    MAX_BOSS2_IDX = 100000,
}


public enum CS
{
    LOGIN,  // 클라이언트가 서버에 접속 요청
    MOVE,   // 이동 확인
    LOGOUT,     // 클라이언트 종료
	CREATE_SESSION,
	SESSION_END,
}

public enum SC
{
    LOGIN_OK,// CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 수락
    LOGIN_FAIL,  // CS_LOGIN의 응답 패킷, 서버에서 클라이언트의 접속을 거절
    POSITION,   // OBJECT의 위치 변경을 클라이언트에 통보
	SET_SESSION_OK,
	ADD_OBJECT,
	END_SESSION,
	REMOVE_OBJECT,
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
	public byte size;
	public char type;
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
	public int sessionId;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]

public class sc_packet_add_object
{
	public byte size;
	public byte type;
	public int id;
	public int hp;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]

public class sc_packet_end_session
{
	public byte size;
	public byte type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]

public class sc_packet_remove_object
{
	public byte size;
	public byte type;
	public int id;
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
	public float move_time;
	public int m_id;
	public cs_packet_move(){}
	public cs_packet_move (byte packet_size, byte packet_type, float posX, float posY, float current_time, int id)
    {
		size = packet_size;
		type = packet_type;
		x = posX;
		y = posY;
		move_time = current_time;
		m_id = id;

	}
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_logout
{
	byte size;
	byte type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_create_session
{
	byte size;
	byte type;
	int sessionType;
	// 어떤 스테이지인지 확인할 수 있는 변수 추가 가능

	public cs_packet_create_session(byte packet_size, byte packet_type, int packet_sessionType)
	{
		size = packet_size;
		type = packet_type;
		sessionType = packet_sessionType;
	}
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_session_end
{
	byte size;
	byte type;

	public cs_packet_session_end(byte packet_size, byte packet_type)
	{
		size = packet_size;
		type = packet_type;
	}
};