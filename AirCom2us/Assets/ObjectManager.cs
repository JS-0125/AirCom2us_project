using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ObjectManager : MonoBehaviour
{
    [SerializeField]
    List<GameObject> Objects;

    [SerializeField]
    List<GameObject> InGameObjects = new List<GameObject>();

    public void AddObject(int id)
    {
        if(id < (int)OBJECT_ID_IDX.MAX_PLAYER_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[0]);
            obj.GetComponent<Object>().id = id;
            InGameObjects.Add(obj);
        }
        else if(id < (int)OBJECT_ID_IDX.MAX_PLANE1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[1]);
            obj.GetComponent<Object>().id = id;

            InGameObjects.Add(obj);
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[2]);
            obj.GetComponent<Object>().id = id;

            InGameObjects.Add(obj);
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_PLANE3_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[3]);
            obj.GetComponent<Object>().id = id;

            InGameObjects.Add(obj);
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS1_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[4]);
            obj.GetComponent<Object>().id = id;

            InGameObjects.Add(obj);
        }
        else if (id < (int)OBJECT_ID_IDX.MAX_BOSS2_IDX + 1)
        {
            var obj = Instantiate<GameObject>(Objects[5]);
            obj.GetComponent<Object>().id = id;

            InGameObjects.Add(obj);
        }
    }
}
