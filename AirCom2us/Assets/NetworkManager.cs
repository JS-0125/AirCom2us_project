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
        // (1) IP �ּҿ� ��Ʈ�� �����ϰ� TCP ���� 
        TcpClient tc = new TcpClient("127.0.0.1", 3500);
    }
}
