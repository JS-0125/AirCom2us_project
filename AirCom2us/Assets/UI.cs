using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI : MonoBehaviour
{
    public void CreateSessionSolo()
    {
        NetworkUtils.SendCreateSessionPacket(1);
    }

    public void CreateSessionMulti()
    {
        NetworkUtils.SendCreateSessionPacket(2);
    }

    public void Join()
    {
        string ip = this.transform.Find("IP").Find("Text").GetComponent<Text>().text;
        if(ip.Length != 0)
            FindObjectOfType<NetworkManager>().StartNetworking(ip);
        else
            FindObjectOfType<NetworkManager>().StartNetworking("192.168.42.94");

    }

    public void RoomJoin()
    {
        FindObjectOfType<NetworkManager>().StartUdpNetworking();
    }
}
