using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Object : MonoBehaviour
{
    [SerializeField]
    private Text hpPrefab;
    private Text hpUi;
    public int id;
    public Vector3 newPos = new Vector3(0, 0, 0);
    [SerializeField]
    private bool dead = false;
    public int hp = -1;

    private void Start()
    {
        hpUi = Instantiate<Text>(hpPrefab, GameObject.Find("Canvas").transform); 
    }

    public void SetObj(int id, int hp)
    {
        this.gameObject.SetActive(false);
        this.id = id;
        this.hp = hp;
    }

    private void OnEnable()
    {
        if (hpUi != null)
            hpUi.gameObject.SetActive(true);
    }

    private void OnDisable()
    {
        if (hpUi != null)
            hpUi.gameObject.SetActive(false);
    }

    private void OnDestroy()
    {
        if (hpUi != null)
            DestroyImmediate(hpUi.gameObject);
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

    public void uiUpdate()
    {
        if (hpUi == null)
            return;
        hpUi.text = hp.ToString();
        hpUi.transform.position = Camera.main.WorldToScreenPoint(this.transform.position);
    }
}
