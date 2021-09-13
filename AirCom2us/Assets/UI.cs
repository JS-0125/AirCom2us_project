using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class UI : MonoBehaviour
{
    public void CreateSession()
    {
        NetworkUtils.SendCreateSessionPacket();
    }

    public void Join()
    {
        string ip = this.transform.Find("IP").Find("Text").GetComponent<Text>().text;
        if(ip.Length != 0)
            FindObjectOfType<NetworkManager>().StartNetworking(ip);
        else
            FindObjectOfType<NetworkManager>().StartNetworking("192.168.42.94");

    }
}
