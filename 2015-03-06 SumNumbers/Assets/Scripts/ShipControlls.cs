using UnityEngine;
using System.Collections;

using Sfs2X;
using Sfs2X.Core;
using Sfs2X.Requests;
using Sfs2X.Entities.Data;

public class ShipControlls : MonoBehaviour {

	SmartFox sfs;

	// Use this for initialization
	void Start () {
		sfs = new SmartFox();
		sfs.ThreadSafeMode = true;

		sfs.AddEventListener(SFSEvent.EXTENSION_RESPONSE, OnExtensionResponse);
	}
	
	// Update is called once per frame
	void Update () {
		sfs.ProcessEvents();

		if (Input.GetButtonDown ("Jump")) {

			//Debug.Log("pressed space");
			sendRequest ();
		}
	}

	void OnExtensionResponse(BaseEvent e) {

		string cmd = (string)e.Params["cmd"];
		ISFSObject objIn = (SFSObject)e.Params["params"];
		
		if (cmd == "Test")
		{
			Debug.Log("Respons: " + objIn.GetInt("NumB"));
		}
	}

	void  sendRequest() {
		ISFSObject objOut = new SFSObject();
		objOut.PutInt("NumA", 1);

		sfs.Send(new ExtensionRequest("Transform", objOut));

		Debug.Log("Sent request");
	}
}
