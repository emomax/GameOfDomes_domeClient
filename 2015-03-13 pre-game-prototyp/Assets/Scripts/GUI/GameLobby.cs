using UnityEngine;
using System.Collections;

public class GameLobby : MonoBehaviour {

	//client variables
	private int roomId;
	private string searchInput;
	private string newRoomName;
	bool showGameCreation;

	public Texture backroundTexture;
	
	//GUI variables
	float mainWindowWidth = 800;
	float mainWindowHeight = Screen.height;
	float buttonWidth = 100;
	float buttonHeight = 50;
	float firstButtonY = 50;
	float buttonVerticalSpacing = 75;
	float textFieldWidth = 150;
	float textFieldHeight = 20;

	//server objects
	SFSHandler sfsScript; //SFS-script
	GameObject serverObject;

	// Use this for initialization
	void Start () {
		//access the server script
		sfsScript = GetComponent<SFSHandler> ();
		serverObject = GameObject.Find("Server");
		sfsScript = (SFSHandler) serverObject.GetComponent(typeof(SFSHandler));

		//default input for text fields
		searchInput = "Seach Game";
		newRoomName = "My Game";
		showGameCreation = false;
	}

	// Update is called once per frame
	void Update () {
	
	}

	void OnGUI() {
		//main background
		GUI.DrawTexture(new Rect(0, 0, Screen.width, Screen.height), backroundTexture);

		//the lobby window
		GUI.Box(new Rect( (Screen.width/2) - mainWindowWidth/2, (Screen.height/2) - mainWindowHeight/2, mainWindowWidth, mainWindowHeight), "Lobby");

		//search game text field
		searchInput = GUI.TextField(new Rect(1150-textFieldWidth/2, 10, textFieldWidth, textFieldHeight), searchInput, 25);

		//join game button
		if (GUI.Button (new Rect (1150-buttonWidth/2, firstButtonY, buttonWidth, buttonHeight), "Join Game")) {
			sfsScript.joinRoom(searchInput);
		}

		//create game button
		if (GUI.Button (new Rect (1150-buttonWidth/2, firstButtonY + buttonVerticalSpacing, buttonWidth, buttonHeight), "Create Game")) {
			showGameCreation = true;
		}

		//Main menu button
		if (GUI.Button (new Rect (1150-buttonWidth/2, firstButtonY + buttonVerticalSpacing*2, buttonWidth, buttonHeight), "Main Menu")) {
			sfsScript.logoutUser();
			Application.LoadLevel("MainMenu");
		}

		//choose name for the room and create
		if (showGameCreation) {

			float boxWidth = Screen.width /5;
			float boxHeight = Screen.height /5;

			GUI.Box(new Rect( (Screen.width/2) - boxWidth/2, (Screen.height/2) - boxHeight/2, boxWidth, boxHeight), "Create Game");

			GUI.SetNextControlName("FocusText"); //set text marker in text area instantly with "Focus"
			newRoomName = GUI.TextField(new Rect(Screen.width/2 - 100, Screen.height/2 - 10, 200, 20), newRoomName, 25);

			Event e = Event.current; // allow player to press enter instead of clicking create
			if (GUI.Button (new Rect (Screen.width/2 - 100, Screen.height/2 + 10, 100, 50), "Create") || (e.isKey && e.keyCode == KeyCode.Return)) {
				sfsScript.createRoom( newRoomName );
			}
			if (GUI.Button (new Rect (Screen.width/2, Screen.height/2 + 10, 100, 50), "Cancel") || (e.isKey && e.keyCode == KeyCode.Escape)) {
				showGameCreation = false;
			}
			GUI.FocusControl("FocusText");
		}
			
	}
}
