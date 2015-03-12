using UnityEngine;
using System.Collections;

public class ProjectileBehaviour : MonoBehaviour {

	//these variables are handled by server and should not be acceced by client
	private float dmg;
	private float velocity = 2;
	private float lifeSpan = 2;
	private Vector3 currentPosition;


	// Use this for initialization
	void Start () {
		currentPosition = transform.position;
	}
	
	// Update is called once per frame
	void Update () {

		translateExtension ();

		destructorExtension ();
	}

	//this funktion will be replaced by a server extension. should only return a new position based on the pilot's current position
	void translateExtension() {

		Vector3 newPosition = currentPosition += gameObject.transform.forward * velocity;// * Time.deltaTime;
		
		//update the spaceship's position with the new position
		updatePosition (newPosition.x, newPosition.y, newPosition.z);
	}

	void destructorExtension() {
		if (lifeSpan <= 0)
			Destroy (gameObject);
		else
			lifeSpan -= Time.deltaTime;
	}

	//client updates the position
	void updatePosition(float _xPos, float _yPos, float _zPos) {
		gameObject.transform.position = new Vector3 (_xPos, _yPos, _zPos);
	}
}
