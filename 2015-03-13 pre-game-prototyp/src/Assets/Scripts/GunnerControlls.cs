using UnityEngine;
using System.Collections;

public class GunnerControlls : MonoBehaviour {

	//these variables can be client-accessed
	public GameObject projectileObject;
	private bool showInGameMenu = false;

	//these variables are handled by server and should not be acceced by client
	private Vector3 currentPosition;
	private Quaternion currentRotation;
	private float fireRate = 0.2f;
	private float fireTimer;
	float rotationSensetivity = 30;

	// Use this for initialization
	void Start () {
		currentRotation = transform.rotation;
		currentPosition = transform.position;
		fireTimer = fireRate;
	}
	
	// Update is called once per frame
	void Update () {

		//update rotation
		rotationExtension (Input.GetButton ("Up"), Input.GetButton ("Down"), Input.GetButton ("Left"), Input.GetButton ("Right"));

		//shoot
		shootExtension (Input.GetButton("Speed"));

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

	//Server gets the input and tells us its ok to shoot
	void shootExtension(bool _fire) {

		if (_fire && fireTimer <= 0) {
			shoot ();
			fireTimer = fireRate;
		}
		else
			fireTimer -= Time.deltaTime;	
	}

	void shoot() {
		//Debug.Log("Fire!");
	
		Instantiate (projectileObject, new Vector3(0, -1, 0), transform.rotation);
	}

	//Used for the in game menu
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
