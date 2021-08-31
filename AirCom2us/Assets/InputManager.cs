using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

using Input = InputWrapper.Input;

public class InputManager : MonoBehaviour
{
    private Vector2 startPoint;
    private Vector2 endPoint;
    private Vector2 currentPoint;

    private void FixedUpdate()
    {
        if (Input.touchCount == 1) {
            if (Input.GetTouch(0).phase == TouchPhase.Began)
            {
                startPoint = Input.GetTouch(0).position;
                print("startPoint Touched" + "x :" + startPoint.x + "y : " + startPoint.y);
            }
            // 터치된 손가락이 그자리에 가만히 있을 때
            else if (Input.GetTouch(0).phase == TouchPhase.Stationary)
            {
                print("Stationary Touched" + "x :" + currentPoint.x + "y : " + currentPoint.y);
            }
            // 터치된 손가락이 움직일 때
            else if (Input.GetTouch(0).phase == TouchPhase.Moved)
            {
                currentPoint = Input.GetTouch(0).position;
                NetworkUtils.SendMovePacket(currentPoint);
                print("currentPoint Touched" + "x :" + currentPoint.x + "y : " + currentPoint.y);
            }
            // 터치된 손가락이 스크린에서 떨어질 때
            else if (Input.GetTouch(0).phase == TouchPhase.Ended)
            {
                endPoint = Input.GetTouch(0).position;
                print("endPoint Touched" + "x :" + endPoint.x + "y : " + endPoint.y);
            }
            // 모바일폰을 귀에 갖다 대거나 touch tracking을 수행하지 않아야 할 경우에
            else if (Input.GetTouch(0).phase == TouchPhase.Canceled) { }
        }
    }
}
