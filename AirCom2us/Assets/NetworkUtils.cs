using System;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using UnityEngine;

public static class NetworkUtils
{
    public static TcpClient tc;

    public static async void Connect()
    {
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        tc = new TcpClient();
        await tc.ConnectAsync("192.168.42.94", 49152);

        if (tc.Connected)
            Debug.Log("Connect OK");
        else
            Debug.Log("Connect Fail");

    }

    public static void Disconnect()
    {
        tc.Close();
    }

    public static void SendMovePacket(Vector2 touchPos)
    {
        Debug.Log("SendMovePacket - " + touchPos.x + ", " + touchPos.y);

        cs_packet_move movePacket = new cs_packet_move((byte)Marshal.SizeOf(typeof(cs_packet_move)), Convert.ToByte(CS.MOVE),touchPos.x,touchPos.y, 0);

        byte[] packet = new byte[1];
        StructToBytes(movePacket, ref packet);

        tc.Client.Send(packet);
    }

    public static void SendLoginPacket()
    {
        Debug.Log("SendLoginPacket");

        cs_packet_login movePacket = new cs_packet_login((byte)Marshal.SizeOf(typeof(cs_packet_login)), Convert.ToByte(CS.LOGIN));

        byte[] packet = new byte[1];
        StructToBytes(movePacket, ref packet);

        tc.Client.Send(packet);
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
