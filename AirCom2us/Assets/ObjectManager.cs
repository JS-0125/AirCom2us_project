using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : MonoBehaviour
{
    [SerializeField]
    List<GameObject> Objects;

    [SerializeField]
    List<Object> InGameObjects = new List<Object>();
    private int playerCnt = 0;
    private int enemyCnt = 0;
    private bool isSessionStart = false;
    private void FixedUpdate()
    {
        if (!isSessionStart)
            return;
        if (CheckSessionEnd())
        {
            isSessionStart = false;
            NetworkUtils.SendSessionEnd();
        }
    }

    public void AddObject(int id, int hp)
    {
        if(id < (int)OBJECT_ID_IDX.MAX_PLAYER_IDX + 1)
        {
            if(InGameObjects.Count == 0)
            {
                var obj = Instantiate<GameObject>(Objects[0]);
                obj.GetComponent<Object>().SetObj(id, hp);
                InGameObjects.Add(obj.GetComponent<Object>());
            }
            else
            {
                var obj = Instantiate<GameObject>(Objects[1]);
                obj.GetComponent<Object>().SetObj(id, hp);
                InGameObjects.Add(obj.GetComponent<Object>());
            }
            ++playerCnt;
        }
        else if(id < (int)OBJECT_ID_IDX.MAX_PLANE1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[2]);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[3]);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE3_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[4]);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[5]);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[6]);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
    }

    public void SetSessionOk()
    {
        foreach (Object obj in InGameObjects)
            obj.gameObject.SetActive(true);
        isSessionStart = true;
    }

    public void MoveObject(int id, float x, float y)
    {
        for(int i = 0; i < InGameObjects.Count; ++i)
            if(InGameObjects[i].id == id)
                InGameObjects[i].newPos = new Vector3(x, y, 0);
    }

    public void EndSession()
    {
        InGameObjects[0].gameObject.SetActive(false);
        for(int i = 1; i < InGameObjects.Count; ++i)
            DestroyImmediate(InGameObjects[i]);
        
        InGameObjects.RemoveRange(1, InGameObjects.Count - 1);
        playerCnt = 0;
        enemyCnt = 0;
    }

    bool CheckSessionEnd()
    {
        int deadPlayerCnt = 0;
        int deadEnemyCnt = 0;
        for (int i = 0; i < InGameObjects.Count; ++i)
        {
            if (InGameObjects[i].isDead())
            {
                if (InGameObjects[i].id < (int)OBJECT_ID_IDX.MAX_PLAYER_IDX + 1)
                    ++deadPlayerCnt;
                else
                    ++deadEnemyCnt;
            }
            if (deadPlayerCnt == playerCnt)
                return true;

            if (deadEnemyCnt == enemyCnt)
                return true;
        }
        return false;
    }
}
