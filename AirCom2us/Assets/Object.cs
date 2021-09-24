using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Object : MonoBehaviour
{
    public int id;
    public Vector3 newPos = new Vector3(0, 0, 0);
    [SerializeField]
    private bool dead = false;
    public int hp = -1;

    public void SetObj(int id, int hp)
    {
        this.gameObject.SetActive(false);
        this.id = id;
        this.hp = hp;
    }

    public void Dead()
    {
        // Á×À½
        dead = true;
        this.gameObject.SetActive(false);
    }

    public bool isDead()
    {
        return dead;
    }
}
