using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Enemy : Object
{
    private void FixedUpdate()
    {
        this.transform.position = Vector3.Lerp(this.transform.position, newPos, 0.1f);
    }
}
