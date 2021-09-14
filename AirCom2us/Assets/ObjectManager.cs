using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : MonoBehaviour
{
    [SerializeField]
    List<GameObject> Objects;

    [SerializeField]
    List<Object> InGameObjects = new List<Object>();

    public void AddObject(int id)
    {
        if(id < (int)OBJECT_ID_IDX.MAX_PLAYER_IDX + 1)
        {
            if(InGameObjects.Count == 0)
            {
                var obj = Instantiate<GameObject>(Objects[0]);
                obj.SetActive(false);
                obj.GetComponent<Object>().id = id;
                InGameObjects.Add(obj.GetComponent<Object>());
            }
            else
            {
                var obj = Instantiate<GameObject>(Objects[1]);
                obj.SetActive(false);
                obj.GetComponent<Object>().id = id;
                InGameObjects.Add(obj.GetComponent<Object>());
            }
        }
        else if(id < (int)OBJECT_ID_IDX.MAX_PLANE1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[2]);
            obj.SetActive(false);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj.GetComponent<Object>());
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[3]);
            obj.SetActive(false);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj.GetComponent<Object>());
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE3_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[4]);
            obj.SetActive(false);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj.GetComponent<Object>());
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[5]);
            obj.SetActive(false);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj.GetComponent<Object>());
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[6]);
            obj.SetActive(false);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj.GetComponent<Object>());
        }
    }

    public void SetSessionOk()
    {
        foreach (Object obj in InGameObjects)
            obj.gameObject.SetActive(true);
    }

    public void EndSession()
    {
        for(int i = 1; i < InGameObjects.Count; ++i)
            DestroyImmediate(InGameObjects[i]);
        
        InGameObjects.RemoveRange(1, InGameObjects.Count - 1);
        InGameObjects[0].gameObject.SetActive(false);
    }

    public void MoveObject(int id, float x, float y)
    {
        for(int i = 0; i < InGameObjects.Count; ++i)
            if(InGameObjects[i].id == id)
                InGameObjects[i].gameObject.transform.position = new Vector3(x, y, 0);
    }
}
