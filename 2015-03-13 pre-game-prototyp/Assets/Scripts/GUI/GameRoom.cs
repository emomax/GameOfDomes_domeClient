using UnityEngine;
using System.Collections;

public class GameRoom : MonoBehaviour {

	//server vaiables
	private bool pilotSelected = false;
	private bool gunnerSelected = false;
	private bool engineerSelected = false;
	private bool roleSelected = false;
	private bool playerReady = false;

	//server objects
	SFSHandler sfsScript; //SFS-script
	GameObject serverObject;

	//GUI
	public Texture backroundTexture;
	GuiCustomStyle guiStyle;

	string role = "";

	// Use this for initialization
	void Start () {

		//access the server script
		sfsScript = GetComponent<SFSHandler> ();
		serverObject = GameObject.Find("Server");
		sfsScript = (SFSHandler) serverObject.GetComponent(typeof(SFSHandler));


		guiStyle = GetComponent<GuiCustomStyle> ();
	}
	
	// Update is called once per frame
	void Update () {

	}
	
	void OnGUI() {

		//Main window
		float mainWindowWidth = 350;
		float mainWindowHeight = 350;

		//button properties
		float buttonWidth = 100;
		float buttonHeight = 50;
		float roleButtonY = Screen.height / 2 - 140;

		//main background
		GUI.DrawTexture(new Rect(0, 0, Screen.width, Screen.height), backroundTexture);

		//the room window
		GUI.Box(new Rect( (Screen.width/2) - mainWindowWidth/2, (Screen.height/2) - mainWindowHeight/2, mainWindowWidth, mainWindowHeight), "Game Room");

		//If player clicks role, a new button replaces the old so that player can deselect
		if (!pilotSelected) {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 - 100, roleButtonY, buttonWidth, buttonHeight), "Pilot", guiStyle.myStyle) && !roleSelected) {
				role = "inGamePilot";
				pilotSelected = true;
				roleSelected = true;
			}
		} else {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 - 100, roleButtonY, buttonWidth, buttonHeight), "Pilot")) {
				role = "";
				pilotSelected = false;
				roleSelected = false;
				playerReady = false;
			}		
		}

		//If player clicks role, a new button replaces the old so that player can deselect
		if (!gunnerSelected) {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, roleButtonY, buttonWidth, buttonHeight), "Gunner", guiStyle.myStyle) && !roleSelected) {
				role = "inGameGunner";
				gunnerSelected = true;
				roleSelected = true;
			}
		} else {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, roleButtonY, buttonWidth, buttonHeight), "Gunner")) {
				role = "";
				gunnerSelected = false;
				roleSelected = false;
				playerReady = false;
			}		
		}

		//If player clicks role, a new button replaces the old so that player can deselect
		if (!engineerSelected) {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 + 100, roleButtonY, buttonWidth, buttonHeight), "Engineer", guiStyle.myStyle) && !roleSelected) {
				role = "";
				engineerSelected = true;
				roleSelected = true;
			}
		} else {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 + 100, roleButtonY, buttonWidth, buttonHeight), "Engineer")) {
				engineerSelected = false;
				roleSelected = false;
				playerReady = false;
			}		
		}

		//chech that all players are ready before starting game
		if (!playerReady) {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 - 100, roleButtonY + 75*3, buttonWidth, buttonHeight), "Ready", guiStyle.myStyle)) {
				if(role != "")
					playerReady = true;
				else
					Debug.Log("Choose role first!");
			}
		} else {
			if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 - 100, roleButtonY + 75*3, buttonWidth, buttonHeight), "Ready")) {
				playerReady = false;
			}		
		}


		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 + 100, roleButtonY + 75*2, buttonWidth, 50), "Start Game")) {
			
			if(playerReady)
				Application.LoadLevel(role);
			else
				Debug.Log("Player not ready");
		}

		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2 + 100, roleButtonY + 75*3, buttonWidth, 50), "Leave Room")) {
			sfsScript.leaveRoom();
		}		
	}
}

