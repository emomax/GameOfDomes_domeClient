﻿using UnityEngine;
using System.Collections;

//smartfox
using Sfs2X;
using Sfs2X.Core;
using Sfs2X.Requests;
using Sfs2X.Entities.Data;

public class SFSHandler : MonoBehaviour {

	SmartFox sfs;

	//smartfox server
	//public string ServerIP = "127.0.0.1";
	//string ServerIP = "85.228.182.184";
	//int ServerPort = 9933;
	string ZoneName = "BasicExamples";
	public string UserName = "";
	//string RoomName = "";
	RoomSettings roomSettings;

	//Client variables
	private string createdRoomName = "";

	bool connectionFail = false;
	bool loginFail = false;

	// Use this for initialization
	void Start () {

	}

	//dont use start since we dont want to reset any server properties when switching scenes
	void Awake() {
		DontDestroyOnLoad(gameObject);

		sfs = new SmartFox();
		sfs.ThreadSafeMode = true;
		
		//add event listeners
		sfs.AddEventListener (SFSEvent.CONFIG_LOAD_SUCCESS, OnConfigLoadSuccessHandler);
		sfs.AddEventListener (SFSEvent.CONNECTION, OnConnection);
		sfs.AddEventListener (SFSEvent.LOGIN, OnLogin);
		sfs.AddEventListener (SFSEvent.LOGIN_ERROR, OnLoginError);
		sfs.AddEventListener (SFSEvent.ROOM_ADD, OnRoomCreated);
		sfs.AddEventListener (SFSEvent.ROOM_CREATION_ERROR, OnRoomCreationError);
		sfs.AddEventListener (SFSEvent.ROOM_JOIN, OnJoinRoom);
		sfs.AddEventListener (SFSEvent.ROOM_JOIN_ERROR, OnJoinRoomError);
		sfs.AddEventListener (SFSEvent.USER_EXIT_ROOM, OnUserExitRoom);

		//load config file on startup, and connect on success
		sfs.LoadConfig("sfs-config.xml", true);
	}

	// Update is called once per frame
	void Update () {
		sfs.ProcessEvents();
	}

	//connect to server if config file successfully loads
	private void OnConfigLoadSuccessHandler(BaseEvent e) {
		Debug.Log("Config file loaded");
		//sfs.Connect(sfs.Config.Host, sfs.Config.Port);
	}

	private void OnConnection(BaseEvent e) {

		bool success = (bool)e.Params["success"];
		if (success) {
			Debug.Log("Successfully Connected");
		} else {
			Debug.Log("Failed to connect");
			connectionFail = true;
		}
	}
	
	private void OnLogin(BaseEvent e) {
		Debug.Log("Logged In: " + e.Params["user"]);
		Application.LoadLevel("GameLobby"); // if login successful, load lobby
	}

	private void OnLoginError(BaseEvent e) {
		Debug.Log("Failed to login");
		loginFail = true;
	}
	
	private void OnRoomCreated(BaseEvent e) {
		Debug.Log("Room created: ");
		joinRoom(createdRoomName); //join the room on creation success
	}

	private void OnRoomCreationError(BaseEvent e) {
		Debug.Log("Room creation failed: ");
	}              

	private void OnJoinRoom(BaseEvent e) {

		Debug.Log("Joined room: " + e.Params["room"]);
		Application.LoadLevel("GameRoom"); // if successful, load room GUI
	}

	private void OnJoinRoomError(BaseEvent e) {
		
		Debug.Log(e.Params["errorMessage"]);
	}

	private void OnUserExitRoom(BaseEvent e) {
		Debug.Log("Left room: " + e.Params["room"]);
		Application.LoadLevel("GameLobby"); //if successfully leaving room, load lobby scene
	}
	
	//connect to server
	public void connectToServer(string _ip) {
		//sfs.Connect(_ip, ServerPort); //with dialogue box
	}

	//login function
	public void loginOnServer(){
		sfs.Send (new LoginRequest (UserName, "", ZoneName));
	}

	public void logoutUser() {
		sfs.Send( new LogoutRequest() );
	}

	// Request to create a new room
	public void createRoom(string roomName) {

		createdRoomName = roomName; // keep this in order to access the room later
		roomSettings = new RoomSettings(roomName);
		sfs.Send (new CreateRoomRequest (roomSettings, false, null));
	}

	//Request to join existing room
	public void joinRoom(string roomName) {
		sfs.Send( new JoinRoomRequest(roomName) );
	}

	//Request to leave current room
	public void leaveRoom() {
		sfs.Send (new LeaveRoomRequest ());
	}

	public void exitGame() {
		sfs.Disconnect();
		System.Diagnostics.Process.GetCurrentProcess ().Kill ();
	}

	void OnGUI() {
		if(connectionFail)
			GUI.Box(new Rect( 0, 50 , 200, 20), "Failed to connect");

		if(loginFail)
			GUI.Box(new Rect( 0, 100 , 200, 20), "Failed to login");
	}
}





