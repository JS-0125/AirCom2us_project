using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class NetworkManager : MonoBehaviour
{

    private void Start()
    {
        test();
    }
    void test()
    {
        // (1) IP 주소와 포트를 지정하고 TCP 연결 
        TcpClient tc = new TcpClient("127.0.0.1", 3500);
    }
}
