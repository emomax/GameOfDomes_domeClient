using UnityEngine;
using System.Collections;

public class MainMenu : MonoBehaviour {

	private bool showNameSelect = false;
	private string userName = "";
	
	public Texture backroundTexture; //background

	//server objects
	SFSHandler sfsScript; //SFS-script
	GameObject serverObject;

	//GUI variables
	float mainWindowWidth = 350;
	float mainWindowHeight = 400;
	float buttonWidth = 300;
	float buttonHeight = 50;
	float firstButtonY = Screen.height/2 - 140;
	float verticalSpacing = 75;

	// Use this for initialization
	void Start () {

		//access the server script
		sfsScript = GetComponent<SFSHandler> ();
		serverObject = GameObject.Find("Server");
		sfsScript = (SFSHandler) serverObject.GetComponent(typeof(SFSHandler));
	}
	
	// Update is called once per frame
	void Update () {
	}

	void OnGUI() {
		//main background
		GUI.DrawTexture(new Rect(0, 0, Screen.width, Screen.height), backroundTexture);

		//menu window background
		GUI.Box(new Rect( (Screen.width/2) - mainWindowWidth/2, (Screen.height/2) - mainWindowHeight/2, mainWindowWidth, mainWindowHeight), "Main Menu");

		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, firstButtonY, buttonWidth, buttonHeight), "Play")) {
			//sfsScript.connectToServer (ipInput);
			showNameSelect = true;
		}
		
		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, firstButtonY + verticalSpacing, buttonWidth, buttonHeight), "Settings")) {
		}
		
		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, firstButtonY + verticalSpacing*2, buttonWidth, buttonHeight), "Credits")) {
		}

		if (GUI.Button (new Rect (Screen.width/2 - buttonWidth/2, firstButtonY + verticalSpacing*3, buttonWidth, buttonHeight), "Exit Game")) {
			sfsScript.exitGame(); 
		}

		//pop up a text field where in game name can be chosen
		if (showNameSelect) {

			float boxWidth = Screen.width /5;
			float boxHeight = Screen.height /5;
			GUI.Box(new Rect( (Screen.width/2) - boxWidth/2, (Screen.height/2) - boxHeight/2, boxWidth, boxHeight), "Username");
			
			GUI.SetNextControlName("FocusText"); //set text marker in text area instantly with "Focus"
			userName = GUI.TextField(new Rect(Screen.width/2 - 100, Screen.height/2 - 10, 200, 20), userName, 25);
			
			Event e = Event.current; // allow player to press enter instead of clicking create
			if (GUI.Button (new Rect (Screen.width/2 - 100, Screen.height/2 + 10, 100, 50), "Continue") || (e.isKey && e.keyCode == KeyCode.Return)) {

				sfsScript.UserName = userName;
				sfsScript.loginOnServer();
			}
			if (GUI.Button (new Rect (Screen.width/2, Screen.height/2 + 10, 100, 50), "Cancel") || (e.isKey && e.keyCode == KeyCode.Escape)) {
				showNameSelect = false;
			}

			GUI.FocusControl("FocusText"); // set marker in text field
		}
	}
}
