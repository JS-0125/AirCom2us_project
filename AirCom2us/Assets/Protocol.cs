using System.Runtime.InteropServices;

//public enum SERVER_PORT 3500;


public enum CS
{
    LOGIN,  // Ŭ���̾�Ʈ�� ������ ���� ��û
    MOVE,   // Ŭ���̾�Ʈ�� �ƹ�Ÿ�� �̵��� ������ ��û
    LOGOUT,		// Ŭ���̾�Ʈ ����
}

public enum SC
{
    LOGIN_OK,// CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
    LOGIN_FAIL,  // CS_LOGIN�� ���� ��Ŷ, �������� Ŭ���̾�Ʈ�� ������ ����
    POSITION,	// OBJECT�� ��ġ ������ Ŭ���̾�Ʈ�� �뺸
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
	byte size;
	char type;
	int id;
	short x, y;
	int HP, LEVEL, EXP;
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
	byte size;
	char type;
	int id;
	short x, y;
	int move_time;          // Stress Test ���α׷����� delay�� ������ �� ���, 
							// ������ �ش� id�� ������ Ŭ���̾�Ʈ���� ������ �ֽ� ���� return �ؾ� �Ѵ�.
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_login
{
	byte size;
	char type;
};

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public class cs_packet_move
{
	byte size;
	byte type;
	int x, y;     // 0:Up, 1:Down, 2:Left, 3:Right
	int move_time;

	public cs_packet_move (byte packet_size, byte packet_type, int posX, int posY, int current_time)
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