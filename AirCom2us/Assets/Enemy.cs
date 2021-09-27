using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : Object
{
    private void FixedUpdate()
    {
        this.transform.position = Vector3.Lerp(this.transform.position, newPos, 0.1f);
        if (hp <= 0)
            Dead();
    }


    void OnBecameInvisible()
    {
        Vector3 viewPos = Camera.main.WorldToScreenPoint(transform.position);
        Debug.Log("vewPos - " + viewPos + "   position - " + transform.position);
        if (viewPos.y < -5)
            Dead();
    }
}
