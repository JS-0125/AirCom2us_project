using System;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Threading;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{
    public static GameState gameState = GameState.LOBBY;
    public static int sessionId;
    private static Thread thrdClientReceive;
    private static Thread thrdUdpClientReceive;

    private int saved_packet_size = 0;
    private Byte[] packet_buffer = new Byte[2048];

    public int playerId = 0;
    private int newPlayerId = 0;
    private string sessionIp;
    private int m_exp;
    private int m_level;
    private int m_hp;

    [SerializeField] private GameObject player;
    [SerializeField] private ObjectManager objectManager;
    private struct sc_data_event
    {
        public SC type;
        public byte[] data;
    }
    private struct data_id
    {
        public int id;
    }
    private struct data_session
    {
        public int sessionId;
        public string str;
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
    private struct data_reconnect_data
    {
        public int prevId;
        public int newId;
    }
    private struct data_collition_result
    {
        public int id;
        public int hp;
        public int exp;
    }

    private Queue<sc_data_event> scDataQueue = new Queue<sc_data_event>();

    private void Start()
    {
        thrdClientReceive = new Thread(new ThreadStart(ListenForData));
        thrdClientReceive.IsBackground = true;

        thrdUdpClientReceive = new Thread(new ThreadStart(UdpListenForData));
        thrdUdpClientReceive.IsBackground = true;
    }

    private void OnDisable()
    {
        NetworkUtils.Disconnect();
        thrdUdpClientReceive.Abort();
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
                        if(gameState == GameState.FREE)
                        {
                            NetworkUtils.SendReconnect(playerId);
                            newPlayerId = playerData.id;
                            break;
                        }
                        playerId = playerData.id;
                        objectManager.AddObject(playerId, playerData.hp);
                        m_hp = playerData.hp;
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
                        object sessionData = new data_session() as object;

                        NetworkUtils.BytesToStructure(data.data, ref sessionData, typeof(data_session));
                        sessionIp = ((data_session)sessionData).str;
                        sessionId = ((data_session)sessionData).sessionId;

                        gameState = GameState.INGAME;

                        StartUdpNetworking();
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
                        gameState = GameState.LOBBY;
                        objectManager.EndSession();
                        EndUdpNetworking();
                        break;
                    }
                case SC.RECONNECT_OK:
                    {
                        object reconnectData = new data_id() as object;
                        NetworkUtils.BytesToStructure(data.data, ref reconnectData, typeof(data_id));
                        Debug.Log("RECONNECT_OK data_id - " + ((data_id)reconnectData).id);
                        if(((data_id)reconnectData).id != -1)
                        {
                            gameState = GameState.INGAME;
                            NetworkUtils.UdpSendReconnectDataPacket(playerId, newPlayerId);
                            objectManager.GetObject(playerId).id = newPlayerId;
                            playerId = newPlayerId;
                        }
                        break;
                    }
                case SC.RECONNECT_DATA:
                    {
                        object reconnectData = new data_reconnect_data() as object;
                        NetworkUtils.BytesToStructure(data.data, ref reconnectData, typeof(data_reconnect_data));
                        var result = (data_reconnect_data)reconnectData;
                        var obj = objectManager.GetObject(result.prevId);
                        obj.id = result.newId;
                        break;
                    }
                case SC.COLLISION_RESULT:
                    {

                        object collisionResult = new data_collition_result() as object;
                        NetworkUtils.BytesToStructure(data.data, ref collisionResult, typeof(data_collition_result));
                        var result = (data_collition_result)collisionResult;
                        Debug.Log("COLLISION_RESULT - " + result.id + "  :  " + result.hp);
                        objectManager.ProcessCollision(result.id, result.hp);
                        m_exp = result.exp;
                        break;
                    }
            }
        }

        if (Application.internetReachability == NetworkReachability.NotReachable)
        {
            if (NetworkUtils.tc != null && gameState != GameState.DISCONNECTED && NetworkUtils.tc.Connected == false)
            {
                Debug.Log("DISCONNECTED");
                gameState = GameState.DISCONNECTED;
                thrdClientReceive.Abort();
                thrdUdpClientReceive.Abort();
                NetworkUtils.Disconnect();
                NetworkUtils.UdpDisconnect();
            }
        }
        if (gameState == GameState.DISCONNECTED)
        {
            if (Application.internetReachability != NetworkReachability.NotReachable)
            {
                // 연결이 되었을 때
                Debug.Log("네트워크 연결!");

                gameState = GameState.FREE;
                saved_packet_size = 0;
                NetworkUtils.TryReConnect();
                NetworkUtils.UdpJoinMulticast();
                thrdClientReceive.Start();
                thrdUdpClientReceive.Start();
            }
        }
    }


    public void StartNetworking(string ip)
    {
        NetworkUtils.Connect(ip);

        NetworkUtils.SendLoginPacket();
        Debug.Log(" tc - " + NetworkUtils.tc.Client.ToString());

        thrdClientReceive.Start();
    }

    public void StartUdpNetworking()
    {
        NetworkUtils.UdpJoinMulticast(sessionIp);
        Debug.Log(" uc - " + NetworkUtils.uc.Client.ToString());

        thrdUdpClientReceive.Start();
    }

    private void EndUdpNetworking()
    {
        thrdUdpClientReceive.Abort();
        NetworkUtils.UdpDisconnect();
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
                if (NetworkUtils.tc.Connected == false)
                    continue;
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
            var data = NetworkUtils.uc.ReceiveAsync();
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
            //Debug.Log("io_byte - " + io_byte);
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
                    m_exp = ((sc_packet_login_ok)packet).exp;
                    m_level = ((sc_packet_login_ok)packet).level;
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
                    //Debug.Log("SC.POSITION");

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
                    
                    //Debug.Log("recv position - id: " + data.id + " " + data.x + ", " + data.y);
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                }
                break;
            case SC.SET_SESSION_OK:
                {
                    Debug.Log("SC.SET_SESSION_OK");

                    object packet = new sc_packet_set_session_ok() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    sc_data_event ev;
                    ev.type = SC.SET_SESSION_OK;
                    ev.data = new byte[1];

                    data_session data = new data_session();
                    data.sessionId = ((sc_packet_set_session_ok)packet).sessionId;
                    data.str = new string(((sc_packet_set_session_ok)packet).ip);

                    Debug.Log(data.str.ToString());

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
            case SC.RECONNECT_OK:
                {
                    Debug.Log("SC.RECONNECT_OK");

                    object packet = new sc_packet_reconnect_ok() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.RECONNECT_OK;
                    ev.data = new byte[1];

                    data_id data = new data_id();
                    data.id = ((sc_packet_reconnect_ok)packet).sessionId;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                    break;
                }
            case SC.COLLISION_RESULT:
                {
                    Debug.Log("SC.COLLISION_RESULT");

                    object packet = new sc_packet_collision_result() as object;
                    NetworkUtils.BytesToStructure(packet_buffer, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.COLLISION_RESULT;
                    ev.data = new byte[1];

                    data_collition_result data = new data_collition_result();
                    data.id = ((sc_packet_collision_result)packet).id;
                    data.hp = ((sc_packet_collision_result)packet).hp;
                    data.exp = ((sc_packet_collision_result)packet).exp;
                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                    break;
                }
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
                    break;
                }
            case CS.RECONNECT:
                {
                    Debug.Log("UDP RECONNECT Data Recv");
                    object packet = new cs_udp_packet_reconnect_data() as object;
                    NetworkUtils.BytesToStructure(bytes, ref packet, packet.GetType());

                    // Enqueue
                    sc_data_event ev;
                    ev.type = SC.RECONNECT_DATA;
                    ev.data = new byte[1];

                    data_reconnect_data data = new data_reconnect_data();
                    data.prevId = ((cs_udp_packet_reconnect_data)packet).prevId;
                    data.newId = ((cs_udp_packet_reconnect_data)packet).newId;

                    NetworkUtils.StructToBytes(data, ref ev.data);

                    scDataQueue.Enqueue(ev);
                    break;
                }
            default:
                Debug.Log("default - " + (SC)bytes[1]);
                break;
        }
    }
}
