using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : Object
{
    [SerializeField]
    private GameObject bulletPrefab;
    private int bulletCnt = 20;
    private GameObject[] bullets = new GameObject[20];
    private int bulletIdx = 1;
    private Coroutine sendPositionCoroutineUdp;
    private Coroutine sendPositionCoroutineTcp;
    private Coroutine bulletCoroutine;

    private void Awake()
    {
        for (int i = 0; i < bulletCnt; ++i)
        {
            bullets[i] = Instantiate<GameObject>(bulletPrefab, new Vector3(0, -5, -5), Quaternion.identity);
            bullets[i].SetActive(false);
        }
    }

    private void FixedUpdate()
    {
        uiUpdate();

        if (hp <= 0)
            Dead();
    }

    private void OnEnable()
    {
        if (NetworkManager.gameState != GameState.INGAME)
            return;
        sendPositionCoroutineUdp = StartCoroutine(SendPositionUdp());
        sendPositionCoroutineTcp = StartCoroutine(SendPositionTcp());
        bulletCoroutine = StartCoroutine(Attack());
        if (hpUi != null)
            hpUi.gameObject.SetActive(true);
    }
    private void OnDisable()
    {
        Debug.Log("hp - " + hp);
        StopAllCoroutines();
        if (hpUi != null)
            hpUi.gameObject.SetActive(false);
        //StopCoroutine(sendPositionCoroutineUdp);
        //StopCoroutine(sendPositionCoroutineTcp);
        //StopCoroutine(bulletCoroutine);
    }
    private IEnumerator Attack()
    {
        while (true)
        {
            bullets[(bulletIdx) % bulletCnt].transform.position = this.gameObject.transform.position;
            bullets[(bulletIdx++) % bulletCnt].SetActive(true);
            yield return new WaitForSeconds(0.5f);
        }
    }


    private IEnumerator SendPositionUdp()
    {
        while (true)
        {
            if(NetworkManager.gameState == GameState.INGAME)
                NetworkUtils.UdpSendMovePacket(this.transform.position, id);
            yield return new WaitForSeconds(0.1f);
        }
    }

    private IEnumerator SendPositionTcp()
    {
        while (true)
        {
            if (NetworkManager.gameState == GameState.INGAME)
                NetworkUtils.SendMovePacketTcp(transform.position, id,Time.time);
            yield return new WaitForSeconds(1f);
        }
    }
}
