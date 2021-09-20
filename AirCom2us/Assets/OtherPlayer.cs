using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OtherPlayer : Object
{
    private void FixedUpdate()
    {
        this.transform.position = Vector3.Lerp(this.transform.position, newPos, 0.1f);
    }
}
