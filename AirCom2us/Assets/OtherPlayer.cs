using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherPlayer : Object
{
    [SerializeField]
    private GameObject bulletPrefab;
    private int bulletCnt = 20;
    private GameObject[] bullets = new GameObject[20];
    private int bulletIdx = 1;
    private Coroutine bulletCoroutine;

    private void Awake()
    {
        for (int i = 0; i < bulletCnt; ++i)
        {
            bullets[i] = Instantiate<GameObject>(bulletPrefab);
            bullets[i].SetActive(false);
        }
    }

    private void OnEnable()
    {
        if (NetworkManager.gameState != GameState.INGAME)
            return;
        bulletCoroutine = StartCoroutine(Attack());
        if (hpUi != null)
            hpUi.gameObject.SetActive(true);
    }

    private void OnDisable()
    {
        StopAllCoroutines();
        if (hpUi != null)
            hpUi.gameObject.SetActive(false);
    }

    private void OnDestroy()
    {
        StopAllCoroutines();
        for (int i = 0; i < bulletCnt; ++i)
        {
            DestroyImmediate(bullets[i]);
        }
    }

    private void FixedUpdate()
    {
        this.transform.position = Vector3.Lerp(this.transform.position, newPos, 0.1f);
        uiUpdate();
        if (hp <= 0)
            Dead();
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
}
