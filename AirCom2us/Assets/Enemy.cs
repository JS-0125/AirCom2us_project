using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : Object
{
    private void Awake()
    {
        newPos = new Vector3(0, 7, 0);
    }
    private void FixedUpdate()
    {
        uiUpdate();
        this.transform.position = Vector3.Lerp(this.transform.position, newPos, 0.1f);
        if (hp <= 0)
            Dead();
    }

    private void OnCollisionEnter2D(Collision2D collision)
    {
        if(collision.gameObject.tag == "bullet")
        {
            if (NetworkManager.gameState == GameState.INGAME)
                NetworkUtils.SendCollicionOccurred(NetworkManager.sessionId, this.id);
            collision.gameObject.SetActive(false);
        }
    }

    private void OnBecameVisible()
    {
        this.gameObject.SetActive(true);
    }
    void OnBecameInvisible()
    {
        Vector3 viewPos = Camera.main.WorldToScreenPoint(transform.position);
        //Debug.Log("vewPos - " + viewPos + "   position - " + transform.position);
        if (viewPos.y < -5)
            Dead();
    }
}
