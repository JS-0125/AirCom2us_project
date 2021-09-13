using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    private static Thread thrdClientReceive;
    private int saved_packet_size = 0;
    private Byte[] packet_buffer = new Byte[1024];

    [SerializeField] private GameObject player;
    [SerializeField] private ObjectManager objectManager;
    private struct sc_data_event
    {
        public SC type;
        public byte[] data;
    }

    private struct data_position
    {
        public float x, y;
    }

    private struct data_add_obejct
    {
        public int id;
    }

    private Queue<sc_data_event> scDataQueue = new Queue<sc_data_event>();

    public void StartNetworking(string ip)
    {
        NetworkUtils.Connect(ip);
        NetworkUtils.SendLoginPacket();

        thrdClientReceive = new Thread(new ThreadStart(ListenForData));
        thrdClientReceive.IsBackground = true;
        thrdClientReceive.Start();
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
                        player.transform.position = new Vector3(((data_position)position).x, ((data_position)position).y, 0);
                    }
                    break;
                case SC.SET_SESSION_OK:
                    {
                        player.SetActive(true);
                        player.transform.position = new Vector3(0, 0, 0);
                    }
                    break;
                case SC.ADD_OBJECT:
                    {
                        object obj = new data_add_obejct() as object;
                        NetworkUtils.BytesToStructure(data.data, ref obj, typeof(data_add_obejct));
                        objectManager.AddObject(((data_add_obejct)obj).id);
                        break;
                    }
            }
        }
    }

    private void OnDisable()
    {
        NetworkUtils.Disconnect();
    }

    private void ListenForData()
    {
        try
        {
            Byte[] buffer = new Byte[1024];
            while (true)
            {
                using (NetworkStream stream = NetworkUtils.tc.GetStream())
                {
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

    private void ProcessData(byte[] buffer, int io_byte)
    {
        byte[] ptr = buffer;
        int ptr_idx = 0;
        int in_packet_size = 0;
        while (0 != io_byte)
        {
            if (0 == in_packet_size) in_packet_size = ptr[0];
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

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.LOGIN_FAIL:
                break;
            case SC.POSITION:
                {
                    object packet = new sc_packet_position() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.POSITION;
                    ev.data = new byte[1];

                    data_position data = new data_position();
                    data.x = ((sc_packet_position)packet).x;
                    data.y = ((sc_packet_position)packet).y;

                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.SET_SESSION_OK:
                {
                    object packet = new sc_packet_position() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.SET_SESSION_OK;
                    ev.data = new byte[1];

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.ADD_OBJECT:
                {
                    object packet = new sc_packet_add_object() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.ADD_OBJECT;
                    ev.data = new byte[1];

                    data_add_obejct data = new data_add_obejct();
                    data.id = ((sc_packet_add_object)packet).id;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            default:
                Debug.Log("default - " + (SC)packet_buffer[1]);
                break;
        }
    }
}
