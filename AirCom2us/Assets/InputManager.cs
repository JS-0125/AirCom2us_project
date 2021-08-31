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
            // ��ġ�� �հ����� ���ڸ��� ������ ���� ��
            else if (Input.GetTouch(0).phase == TouchPhase.Stationary)
            {
                print("Stationary Touched" + "x :" + currentPoint.x + "y : " + currentPoint.y);
            }
            // ��ġ�� �հ����� ������ ��
            else if (Input.GetTouch(0).phase == TouchPhase.Moved)
            {
                currentPoint = Input.GetTouch(0).position;
                NetworkUtils.SendMovePacket(currentPoint);
                print("currentPoint Touched" + "x :" + currentPoint.x + "y : " + currentPoint.y);
            }
            // ��ġ�� �հ����� ��ũ������ ������ ��
            else if (Input.GetTouch(0).phase == TouchPhase.Ended)
            {
                endPoint = Input.GetTouch(0).position;
                print("endPoint Touched" + "x :" + endPoint.x + "y : " + endPoint.y);
            }
            // ��������� �Ϳ� ���� ��ų� touch tracking�� �������� �ʾƾ� �� ��쿡
            else if (Input.GetTouch(0).phase == TouchPhase.Canceled) { }
        }
    }
}
