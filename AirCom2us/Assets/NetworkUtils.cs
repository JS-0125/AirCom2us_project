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

    public static async void Connect(string ip)
    {
        networkIp = ip;
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        tc = new TcpClient();
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

    public static void Disconnect()
    {
        tc.Close();
    }

    public static void SendMovePacket(Vector2 touchPos)
    {
        //Debug.Log("SendMovePacket - " + touchPos.x + ", " + touchPos.y);

        cs_packet_move movePacket = new cs_packet_move((byte)Marshal.SizeOf(typeof(cs_packet_move)), Convert.ToByte(CS.MOVE),touchPos.x,touchPos.y, 0);

        SendPacket(ref movePacket);
        //byte[] packet = new byte[1];
        //StructToBytes(movePacket, ref packet);

        //tc.Client.Send(packet);
    }

    public static void UdpSendMovePacket(Vector2 touchPos)
    {
        Debug.Log("SendMovePacket - " + touchPos.x + ", " + touchPos.y);

        cs_packet_move movePacket = new cs_packet_move((byte)Marshal.SizeOf(typeof(cs_packet_move)), Convert.ToByte(CS.MOVE), touchPos.x, touchPos.y, 0);

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

    private static void SendPacket<T>(ref T data)
    {
        byte[] packet = new byte[1];
        StructToBytes(data, ref packet);

        tc.Client.Send(packet);
    }

    private static void UdpSendPacket<T>(ref T data)
    {
        byte[] packet = new byte[1];
        StructToBytes(data, ref packet);

        uc.Send(packet, Marshal.SizeOf(data), networkIp, udpPort);
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
