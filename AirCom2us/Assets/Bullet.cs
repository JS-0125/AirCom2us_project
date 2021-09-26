using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Bullet : MonoBehaviour
{
    void FixedUpdate()
    {
        var pos = this.transform.position;
        this.transform.position = new Vector3(pos.x, pos.y + 0.1f, 0);
    }

    void OnBecameInvisible()
    {
        this.gameObject.SetActive(false);
    }
}
