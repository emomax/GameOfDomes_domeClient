using UnityEngine;
using System.Collections;
using Sfs2X;
using Sfs2X.Core;
using Sfs2X.Requests;
using Sfs2X.Entities.Data;

public class SFS2X_Connect : MonoBehaviour {
	
	public string ServerIP = "127.0.0.1";
	public int ServerPort = 9933;
	public string ZoneName = "BasicExamples";
	public string UserName = "";
	
	SmartFox sfs;
	
	void Start()
	{
		sfs = new SmartFox();
		sfs.ThreadSafeMode = true;
		
		sfs.Connect(ServerIP, ServerPort);
		
		sfs.AddEventListener(SFSEvent.CONNECTION, OnConnection);
		sfs.AddEventListener(SFSEvent.LOGIN, OnLogin);
		sfs.AddEventListener(SFSEvent.EXTENSION_RESPONSE, OnExtensionResponse);
	}
	
	void OnLogin(BaseEvent e)
	{
		Debug.Log("Logged In: " + e.Params["user"]);
		ISFSObject objOut = new SFSObject();
		objOut.PutInt("NumA", 2);
		objOut.PutInt("NumB", 5);
		
		sfs.Send(new ExtensionRequest("SumNumbers", objOut));
	}
	
	void OnExtensionResponse(BaseEvent e)
	{
		string cmd = (string)e.Params["cmd"];
		ISFSObject objIn = (SFSObject)e.Params["params"];
		
		if (cmd == "SumNumbers")
		{
			Debug.Log("Sum: " + objIn.GetInt("NumC"));
		}
	}
	
	void OnConnection(BaseEvent e)
	{
		if ((bool)e.Params["success"])
		{
			Debug.Log("Successfully Connected");
			sfs.Send(new LoginRequest(UserName, "", ZoneName));
		} else {
			Debug.Log("Connection Failed");
		}
	}
	
	void Update()
	{
		sfs.ProcessEvents();
	}
	
	void OnApplicationQuit()
	{
		if (sfs.IsConnected)
			sfs.Disconnect();
	}
}