using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

using Input = InputWrapper.Input;

public class PlayerInput : MonoBehaviour
{
    private Vector2 startPoint;
    private Vector2 endPoint;
    private Vector2 currentPoint;
    private int playerId;

    private void Start()
    {
        playerId = GameObject.FindObjectOfType<NetworkManager>().playerId;
    }
    private void FixedUpdate()
    {
        if (Input.touchCount == 1)
        {
            Touch touch = Input.GetTouch(0);
            switch (touch.phase)
            {
                case TouchPhase.Began:
                    //Debug.Log("TouchPhase Began!");
                    startPoint = touch.position;
                    break;
                case TouchPhase.Moved:
                    {
                        //Debug.Log("TouchPhase Moved!" + currentPoint.x +", " + currentPoint.y);
                        currentPoint = touch.position;
                        //NetworkUtils.SendMovePacket(Camera.main.ScreenToWorldPoint(touch.position));
                        var worldPos = Camera.main.ScreenToWorldPoint(touch.position);
                        //NetworkUtils.UdpSendMovePacket(worldPos, playerId);
                        this.gameObject.transform.position = new Vector3(worldPos.x, worldPos.y, 0);
                        break;
                    }
                case TouchPhase.Stationary:
                    {
                        //Debug.Log("TouchPhase Stationary!");
                        if (Mathf.Abs(currentPoint.x - touch.position.x) < 0.001
                            && Mathf.Abs(currentPoint.y - touch.position.y) < 0.001)
                        {
                            //Debug.Log("currentPoint == Input.GetTouch(0).position - don't send");
                            return;
                        }
                        currentPoint = touch.position;
                        //NetworkUtils.SendMovePacket(Camera.main.ScreenToWorldPoint(touch.position));
                        var worldPos = Camera.main.ScreenToWorldPoint(touch.position);
                        //NetworkUtils.UdpSendMovePacket(worldPos, playerId);
                        this.gameObject.transform.position = new Vector3(worldPos.x, worldPos.y, 0);
                        break;
                    }
                case TouchPhase.Ended:
                    //Debug.Log("TouchPhase Ended!");
                    endPoint = touch.position;
                    break;
                case TouchPhase.Canceled:
                    //Debug.Log("TouchPhase Canceled!");
                    break;
            }
        }
    }
}
