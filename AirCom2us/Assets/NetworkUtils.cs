using System;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;

public static class NetworkUtils
{
    public static TcpClient tc;
    public static UdpClient uc;
    private static string networkIp;
    private static int port= 49152;
    private static int udpPort= 49151;
    private static IPEndPoint remoteEP;
    private static IPAddress multicastIP;
    private static IPEndPoint localEP;
    public static async void Connect(string ip)
    {
        networkIp = ip;
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        tc = new TcpClient();
        tc.NoDelay = true;

        await tc.ConnectAsync(networkIp, port);

        if (tc.Connected)
            Debug.Log("Connect OK");
        else
            Debug.Log("Connect Fail");

    }

    public static void UdpConnect(string ip)
    {
        uc = new UdpClient();
        //uc.Connect(ip, 49152);
        // 나중에 멀티캐스트나 브로드캐스트 하기
    }

    public static void UdpJoinMulticast(string ip)
    {
        // (1) UdpClient 객체 성성
        uc = new UdpClient(udpPort);

        // (2) UDP 로컬 IP/포트에 바인딩            
        // udp.Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.ReuseAddress, true);
        //localEP = new IPEndPoint(IPAddress.Any, udpPort);
        //uc.Client.Bind(localEP);

        // (3) Multicast 그룹에 Join
        multicastIP = IPAddress.Parse(ip);
        uc.JoinMulticastGroup(multicastIP);

        remoteEP = new IPEndPoint(multicastIP, udpPort);
    }

    public static void UdpDisconnect()
    {
        uc.Close();
    }

    public static void Disconnect()
    {
        tc.Close();
    }

    public static void SendMovePacketTcp(Vector2 touchPos, int id, float time)
    {
        Debug.Log("SendMovePacketTcp - " + time);

        cs_packet_move movePacket = new cs_packet_move((byte)Marshal.SizeOf(typeof(cs_packet_move)), Convert.ToByte(CS.MOVE), touchPos.x, touchPos.y, time, id);

        SendPacket(ref movePacket);
    }

    public static void UdpSendMovePacket(Vector2 touchPos, int id)
    {
        //Debug.Log("SendMovePacket - " + touchPos.x + ", " + touchPos.y);

        cs_packet_move movePacket = new cs_packet_move((byte)Marshal.SizeOf(typeof(cs_packet_move)), Convert.ToByte(CS.MOVE), touchPos.x, touchPos.y, Time.time, id);

        UdpSendPacket(ref movePacket);
    }

    public static void SendLoginPacket()
    {
        Debug.Log("SendLoginPacket");

        cs_packet_login loginPacket = new cs_packet_login((byte)Marshal.SizeOf(typeof(cs_packet_login)), Convert.ToByte(CS.LOGIN));

        SendPacket(ref loginPacket);
    }

    public static void SendCreateSessionPacket(int sessionType)
    {
        Debug.Log("SendCreateSessionPacket");

        cs_packet_create_session createSessionPacket = new cs_packet_create_session((byte)Marshal.SizeOf(typeof(cs_packet_create_session)), Convert.ToByte(CS.CREATE_SESSION), sessionType);

        SendPacket(ref createSessionPacket);
    }

    public static void SendSessionEnd() {
        Debug.Log("SendSessionEnd");

        cs_packet_session_end sessionEndPacket = new cs_packet_session_end((byte)Marshal.SizeOf(typeof(cs_packet_session_end)), Convert.ToByte(CS.SESSION_END));

        SendPacket(ref sessionEndPacket);
    }

    private static void SendPacket<T>(ref T data)
    {
        byte[] packet = new byte[1];
        StructToBytes(data, ref packet);

        tc.Client.Send(packet);
    }

    private static void UdpSendPacket<T>(ref T data)
    {
        if (uc == null)
        {
            //Debug.Log("udp is null");
            return;
        }
        byte[] packet = new byte[1];
        StructToBytes(data, ref packet);

        uc.Send(packet, Marshal.SizeOf(data), remoteEP);
    }

    public static void StructToBytes(object obj, ref byte[] packet)
    {
        int size = Marshal.SizeOf(obj);
        packet = new byte[size];
        IntPtr buffer = Marshal.AllocHGlobal(size + 1);
        Marshal.StructureToPtr(obj, buffer, false);
        Marshal.Copy(buffer, packet, 0, size);
        Marshal.FreeHGlobal(buffer);
    }

    public static void BytesToStructure(byte[] bValue, ref object obj, Type t)
    {
        int size = Marshal.SizeOf(t);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        Marshal.Copy(bValue, 0, buffer, size);
        obj = Marshal.PtrToStructure(buffer, t);
        Marshal.FreeHGlobal(buffer);
    }
}
