using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Object
{
    private Coroutine sendPositionCoroutineUdp;
    private Coroutine sendPositionCoroutineTcp;

    private void FixedUpdate()
    {
        if (hp <= 0)
            Dead();
    }

    private void OnEnable()
    {
        sendPositionCoroutineUdp = StartCoroutine(SendPositionUdp());
        sendPositionCoroutineTcp = StartCoroutine(SendPositionTcp());
    }
    private void OnDisable()
    {
        StopCoroutine(sendPositionCoroutineUdp);
        StopCoroutine(sendPositionCoroutineTcp);
    }

    private IEnumerator SendPositionUdp()
    {
        while (true)
        {
            NetworkUtils.UdpSendMovePacket(this.transform.position, id);
            yield return new WaitForSeconds(0.1f);
        }
    }

    private IEnumerator SendPositionTcp()
    {
        while (true)
        {
            NetworkUtils.SendMovePacketTcp(transform.position, id,Time.time);
            yield return new WaitForSeconds(1f);
        }
    }
}
