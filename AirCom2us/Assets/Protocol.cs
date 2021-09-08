using System.Runtime.InteropServices;

//public enum SERVER_PORT 3500;


public enum CS
{
    LOGIN,  // Ŭ���̾�Ʈ�� ������ ���� ��û
    MOVE,   // Ŭ���̾�Ʈ�� �ƹ�Ÿ�� �̵��� ������ ��û
    LOGOUT,     // Ŭ���̾�Ʈ ����
	CREATE_SESSION,
}

public enum SC
{
    LOGIN_OK,// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
    LOGIN_FAIL,  // CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
    POSITION,   // OBJECT�� ��ġ ������ Ŭ���̾�Ʈ�� �뺸
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
	public int move_time;          // Stress Test ���α׷����� delay�� ������ �� ���, 
							// ������ �ش� id�� ������ Ŭ���̾�Ʈ���� ������ �ֽ� ���� return �ؾ� �Ѵ�.
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
	// � ������������ Ȯ���� �� �ִ� ���� �߰� ����

	public cs_packet_create_session(byte packet_size, byte packet_type)
	{
		size = packet_size;
		type = packet_type;
	}
};