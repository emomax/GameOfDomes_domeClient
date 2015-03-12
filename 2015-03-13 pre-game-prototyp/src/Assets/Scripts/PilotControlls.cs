using UnityEngine;
using System.Collections;

public class PilotControlls : MonoBehaviour {

	//client variables
	private bool showInGameMenu = false;

	//these variables are handled by server and should not be acceced by client
	private float velocity = 0;
	private float maxVelocity = 5;
	private float accellerationAmount = .1f;
	private Vector3 currentPosition;
	float rotationSensetivity = 30;

	// Use this for initialization
	void Start () {
	
		currentPosition = transform.position;
	}
	
	// Update is called once per frame
	void Update () {

		//update rotation
		rotationExtension (Input.GetButton ("Up"), Input.GetButton ("Down"), Input.GetButton ("Left"), Input.GetButton ("Right"));

		//update speed
		translateExtension (Input.GetButton ("Speed"), Input.GetButton ("Slow"));

		//in game menu
		if (Input.GetButtonDown ("Cancel") && !showInGameMenu) {
			showInGameMenu = true;
		} else if (Input.GetButtonDown ("Cancel") && showInGameMenu) {
			showInGameMenu = false;
		}

		//temporary controlls not taking server data into consideration
		float horizontalRotation = Input.GetAxis ("Horizontal") * rotationSensetivity;
		float verticalRotation = Input.GetAxis ("Vertical") * rotationSensetivity;
		transform.Rotate (verticalRotation * Time.deltaTime, horizontalRotation * Time.deltaTime, 0);
	}

	//rotate the ship. gets input and returns rotation (this function will be replaced by a server extension)
	void rotationExtension(bool _up, bool _down, bool _left, bool _right) {

	}

	//translate the ship. gets input and returns position (this function will be replaced by a server extension)
	void translateExtension(bool _speed, bool _slow) {
		if (_speed) {
			velocity += accellerationAmount;
			if (velocity > maxVelocity)
				velocity = maxVelocity;
		}
		if (_slow) {
			velocity -= accellerationAmount;	
			if (velocity < -maxVelocity)
				velocity = -maxVelocity;
		}

		Vector3 newPosition = currentPosition += gameObject.transform.forward * velocity * Time.deltaTime;

		//update the spaceship's position with the new position
		updatePosition (newPosition.x, newPosition.y, newPosition.z);
	}

	//use new data to update rotation
	void updateRotation(float _xRot, float _yRot, float _zRot) {
		Vector3 _forward = new Vector3 (_xRot, _yRot, _zRot);
		Quaternion _newRotation = Quaternion.LookRotation(_forward, Vector3.up);
		transform.rotation = _newRotation;
	}

	//use new data to update position
	void updatePosition(float _xPos, float _yPos, float _zPos) {
		gameObject.transform.position = new Vector3 (_xPos, _yPos, _zPos);
	}

	void OnGUI() {

		//In game menu window properties
		float mainWindowWidth = 175;
		float mainWindowHeight = 250;

		//button properties
		float buttonWidth = 100;
		float buttonHeight = 50;
		float firstButtonY = Screen.height / 2 - 60;
		float verticalButtonSpacing = 75;

		if (showInGameMenu) {

			//in game menu window
			GUI.Box(new Rect( (Screen.width/2) - mainWindowWidth/2, (Screen.height/2) - mainWindowHeight/2, mainWindowWidth, mainWindowHeight), "Pause");
			
			if (GUI.Button (new Rect (Screen.width/2-buttonWidth/2, firstButtonY, buttonWidth, buttonHeight), "Settings")) {
			}
			if (GUI.Button (new Rect (Screen.width/2-buttonWidth/2, firstButtonY+verticalButtonSpacing, buttonWidth, buttonHeight), "Abandon Game")) {
				Application.LoadLevel("GameLobby");
			}
		}
	}
}
