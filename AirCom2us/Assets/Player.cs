using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Object
{
    private Coroutine sendPositionCoroutine;
    private void OnEnable()
    {
        sendPositionCoroutine = StartCoroutine(SendPosition());
    }
    private void OnDisable()
    {
        StopCoroutine(sendPositionCoroutine);
    }

    private IEnumerator SendPosition()
    {
        while (true)
        {
            NetworkUtils.UdpSendMovePacket(this.transform.position, id);
            yield return new WaitForSeconds(0.1f);
        }
    }
}
