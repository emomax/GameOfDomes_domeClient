using UnityEngine;
using System.Collections;

public class PilotControlls : MonoBehaviour {

	//client variables
	private bool showInGameMenu = false;

	//input variables
	private int verticalInput;
	private int horizontalInput;
	private int thrustInput;

	//server objects
	SFSHandler sfsScript; //SFS-script
	GameObject serverObject;
	
	// Use this for initialization
	void Start () {

		//access the server script
		sfsScript = GetComponent<SFSHandler> ();
		serverObject = GameObject.Find("Server");
		sfsScript = (SFSHandler) serverObject.GetComponent(typeof(SFSHandler));
	}
	
	// Update is called once per frame
	void Update () {

		//horizontal input
		if (Input.GetButton ("Right"))
			horizontalInput = 1;
		else if (Input.GetButton ("Left"))
			horizontalInput = -1;
		else
			horizontalInput = 0;

		//vertical input
		if (Input.GetButton ("Up"))
			verticalInput = 1;
		else if (Input.GetButton ("Down"))
			verticalInput = -1;
		else
			verticalInput = 0;

		//thrust input
		if (Input.GetButton ("Speed"))
			thrustInput = 1;
		else if (Input.GetButton ("Slow"))
			thrustInput = -1;
		else
			thrustInput = 0;

		//send input data to server
		if(!(horizontalInput == 0 && verticalInput == 0 && thrustInput == 0))
			sfsScript.sendDataToServer (horizontalInput, verticalInput, thrustInput);
		
		//toggle in game menu
		if (Input.GetButtonDown ("Cancel") && !showInGameMenu) {
			showInGameMenu = true;
		} else if (Input.GetButtonDown ("Cancel") && showInGameMenu) {
			showInGameMenu = false;
		}
	}

	//use new data to update position
	public void updatePosition(double _xPos, double _yPos, double _zPos) {
		transform.position = new Vector3 ((float)_xPos, (float)_yPos, (float)_zPos);
	}

	//use new data to update rotation
	public void updateRotation(double _yRot, double _xRot, double _zRot) {
		transform.localEulerAngles = new Vector3((float)_xRot, (float)_yRot, (float)_zRot);
	}

	//gui used for in game menu
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

				sfsScript.leaveRoom();
			}
		}
	}
}
