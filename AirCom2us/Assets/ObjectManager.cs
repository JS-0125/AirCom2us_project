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
            var obj = Instantiate<GameObject>(Objects[2], new Vector3(0, 6, 0), Quaternion.identity);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[3], new Vector3(0, 6, 0), Quaternion.identity);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE3_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[4], new Vector3(0, 6, 0), Quaternion.identity);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[5], new Vector3(0, 6, 0), Quaternion.identity);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[6], new Vector3(0, 6, 0), Quaternion.identity);
            obj.GetComponent<Object>().SetObj(id, hp);
            InGameObjects.Add(obj.GetComponent<Object>());
            ++enemyCnt;
        }
    }
    public Object GetObject(int id)
    {
        for (int i = 0; i < InGameObjects.Count; ++i)
            if (InGameObjects[i].id == id)
                return InGameObjects[i];
        return null;
    }

    public void RemoveObject(int id)
    {
        for(int i = 0; i < InGameObjects.Count; ++i)
        {
            if(InGameObjects[i].id == id)
            {
                if (InGameObjects[i].id < (int)OBJECT_ID_IDX.MAX_PLAYER_IDX + 1)
                    --playerCnt;
                else
                    --enemyCnt;
                DestroyImmediate(InGameObjects[i].gameObject);
                InGameObjects.RemoveAt(i);
            }
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

    public void ProcessCollision(int id, int hp)
    {
        for (int i = 0; i < InGameObjects.Count; ++i)
            if (InGameObjects[i].id == id)
                InGameObjects[i].hp = hp;
    }


    public void EndSession()
    {
        InGameObjects[0].gameObject.SetActive(false);
        for(int i = 1; i < InGameObjects.Count; ++i)
            DestroyImmediate(InGameObjects[i].gameObject);
        
        InGameObjects.RemoveRange(1, InGameObjects.Count - 1);
        playerCnt = 1;
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
