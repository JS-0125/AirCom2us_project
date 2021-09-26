using System;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    private static Thread thrdClientReceive;
    private static Thread thrdUdpClientReceive;

    private int saved_packet_size = 0;
    private Byte[] packet_buffer = new Byte[2048];

    public int playerId;

    [SerializeField] private GameObject player;
    [SerializeField] private ObjectManager objectManager;
    [SerializeField] private int sessionId;
    private struct sc_data_event
    {
        public SC type;
        public byte[] data;
    }
    private struct data_id
    {
        public int id;
    }

    private struct data_position
    {
        public int id;
        public float x, y;
    }

    private struct data_obj
    {
        public int id;
        public int hp;
    }


    private Queue<sc_data_event> scDataQueue = new Queue<sc_data_event>();

    public void StartNetworking(string ip)
    {
        NetworkUtils.Connect(ip);

        Debug.Log(" tc - " + NetworkUtils.tc.Client.ToString());
        NetworkUtils.SendLoginPacket();

        thrdClientReceive = new Thread(new ThreadStart(ListenForData));
        thrdClientReceive.IsBackground = true;
        thrdClientReceive.Start();
    }

    public void StartUdpNetworking()
    {
        NetworkUtils.UdpJoinMulticast();
        Debug.Log(" uc - " + NetworkUtils.uc.Client.ToString());

        thrdUdpClientReceive = new Thread(new ThreadStart(UdpListenForData));
        thrdUdpClientReceive.IsBackground = true;
        thrdUdpClientReceive.Start();
    }

    private void Update()
    {
        if (scDataQueue.Count > 0)
        {
            var data = scDataQueue.Dequeue();
            switch (data.type)
            {
                case SC.LOGIN_OK:
                    {
                        Debug.Log("SC.LOGIN_OK");
                        object obj = new data_obj() as object;
                        NetworkUtils.BytesToStructure(data.data, ref obj, typeof(data_obj));
                        
                        var playerData = (data_obj)obj;
                        playerId = playerData.id;
                        objectManager.AddObject(playerId, playerData.hp);

                        //player.SetActive(true);
                        //player.transform.position = new Vector3(data.x, data.y, 0);
                    }
                    break;
                case SC.LOGIN_FAIL:
                    break;
                case SC.POSITION:
                    {
                        object position = new data_position() as object;
                        NetworkUtils.BytesToStructure(data.data, ref position, typeof(data_position));
                        objectManager.MoveObject(((data_position)position).id, ((data_position)position).x, ((data_position)position).y);
                        //player.transform.position = new Vector3(((data_position)position).x, ((data_position)position).y, 0);
                    }
                    break;
                case SC.SET_SESSION_OK:
                    {
                        //player.SetActive(true);
                        //player.transform.position = new Vector3(0, 0, 0);
                        object objId = new data_id() as object;
                        NetworkUtils.BytesToStructure(data.data, ref objId, typeof(data_id));
                        sessionId = ((data_id)objId).id;
                        objectManager.SetSessionOk();
                    }
                    break;
                case SC.ADD_OBJECT:
                    {
                        object objId = new data_obj() as object;
                        NetworkUtils.BytesToStructure(data.data, ref objId, typeof(data_obj));
                        objectManager.AddObject(((data_obj)objId).id, ((data_obj)objId).hp);
                        break;
                    }
                case SC.REMOVE_OBJECT:
                    {
                        object objId = new data_obj() as object;
                        NetworkUtils.BytesToStructure(data.data, ref objId, typeof(data_obj));
                        objectManager.RemoveObject(((data_obj)objId).id);
                        break;
                    }
                case SC.END_SESSION:
                    { 
                        objectManager.EndSession();
                        break;
                    }
            }
        }
    }

    private void OnDisable()
    {
        NetworkUtils.Disconnect();
    }

    public void OnApplicationPause(bool paused)
    {
        if (paused)
        {
            //Game is paused, start service to get notifications
        }
        else
        {
            //Game is unpaused, stop service notifications.
            // 장시간 puase되었다가 resume될 경우 연결이 끊어질 수 있으므로 상태를 체크한다.
        }
    }

        private void ListenForData()
    {
        try
        {
            while (true)
            {
                using (NetworkStream stream = NetworkUtils.tc.GetStream())
                {
                    Byte[] buffer = new Byte[512];
                    int length;
                    while ((length = stream.Read(buffer, 0, buffer.Length)) != 0)
                    {
                        ProcessData(buffer, length);
                    }
                }
            }
        }

        catch (SocketException ex)
        {
            Debug.Log(ex);
        }
    }

    private void UdpListenForData()
    {
        while (true)
        {
            //Debug.Log("UdpListenForData");
            var data = NetworkUtils.uc.ReceiveAsync();
            //Debug.Log("data.Result.Buffer.Length - " + data.Result.Buffer.Length);

            ProcessUdpPacket(data.Result.Buffer);
        }
    }


    private void ProcessData(byte[] buffer, int io_byte)
    {
        byte[] ptr = buffer;
        int ptr_idx = 0;
        int in_packet_size = 0;
        while (0 != io_byte)
        {
            Debug.Log("io_byte - " + io_byte);
            if (0 == in_packet_size) in_packet_size = ptr[ptr_idx];
            if (io_byte + saved_packet_size >= in_packet_size)
            {
                Array.Copy(ptr, ptr_idx, packet_buffer, saved_packet_size, in_packet_size - saved_packet_size);
                ProcessPacket();
                ptr_idx += in_packet_size - saved_packet_size;
                io_byte -= in_packet_size - saved_packet_size;
                in_packet_size = 0;
                saved_packet_size = 0;
            }
            else
            {
                Array.Copy(ptr, ptr_idx, packet_buffer, saved_packet_size, io_byte);
                saved_packet_size += io_byte;
                io_byte = 0;
            }
        }
    }

    private void ProcessPacket()
    {
        // deserialize
        switch ((SC)packet_buffer[1])
        {
            case SC.LOGIN_OK:
                {
                    Debug.Log("Login OK");
                    object packet = new sc_packet_login_ok() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.LOGIN_OK;
                    ev.data = new byte[1];

                    data_obj data = new data_obj();
                    data.id = ((sc_packet_login_ok)packet).id;

                    // 임의 hp
                    data.hp = 100;

                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.LOGIN_FAIL:
                break;
            case SC.POSITION:
                {
                    Debug.Log("SC.POSITION");

                    object packet = new sc_packet_position() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.POSITION;
                    ev.data = new byte[1];

                    data_position data = new data_position();
                    data.id = ((sc_packet_position)packet).id;
                    data.x = ((sc_packet_position)packet).x;
                    data.y = ((sc_packet_position)packet).y;
                    
                    Debug.Log("recv position - id: " + data.id + " " + data.x + ", " + data.y);
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.SET_SESSION_OK:
                {
                    Debug.Log("SC.SET_SESSION_OK");

                    object packet = new sc_packet_set_session_ok() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.SET_SESSION_OK;
                    ev.data = new byte[1];

                    data_id data = new data_id();
                    data.id = ((sc_packet_set_session_ok)packet).sessionId;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.ADD_OBJECT:
                {
                    Debug.Log("SC.ADD_OBJECT");

                    object packet = new sc_packet_add_object() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.ADD_OBJECT;
                    ev.data = new byte[1];

                    data_obj data = new data_obj();
                    data.id = ((sc_packet_add_object)packet).id;
                    data.hp = ((sc_packet_add_object)packet).hp;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.REMOVE_OBJECT:
                {
                    Debug.Log("SC.REMOVE_OBJECT");

                    object packet = new sc_packet_remove_object() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.REMOVE_OBJECT;
                    ev.data = new byte[1];

                    data_id data = new data_id();
                    data.id = ((sc_packet_remove_object)packet).id;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.END_SESSION:
                {
                    Debug.Log("SC.END_SESSION");
                    object packet = new sc_packet_end_session() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.END_SESSION;
                    ev.data = new byte[1];

                    scDataQueue.Enqueue(ev);
                }
                break;
            default:
                Debug.Log("default - " + (int)packet_buffer[1] + " data size: " + (int)packet_buffer[0]);
                break;
        }
    }

    private void ProcessUdpPacket(byte[] bytes)
    {
        switch ((CS)bytes[1])
        {
            case CS.MOVE:
                {
                    Debug.Log("udp - " + (CS)bytes[1]);

                    object packet = new cs_packet_move() as object;
                    NetworkUtils.BytesToStructure(bytes, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.POSITION;
                    ev.data = new byte[1];

                    data_position data = new data_position();
                    data.id = ((cs_packet_move)packet).m_id;
                    data.x = ((cs_packet_move)packet).x;
                    data.y = ((cs_packet_move)packet).y;

                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            default:
                Debug.Log("default - " + (SC)bytes[1]);
                break;
        }
    }
}
