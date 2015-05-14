/*
osgExample_sfs
*/

#include "sgct.h"

/* Custom items */
#include "classroom\Includes.h"
#include "classroom\Utilities.h"
#include "classroom\Projectile.h"
#include "classroom\EnemyShip.h"
#include "classroom\Player.h"
#include "classroom\NetworkManager.h"
#include "classroom\SoundManager.h"

//Engine handling everything SGCT-related
sgct::Engine * gEngine;

//Not using ref pointers enables more controlled
//termination and prevents segfault on Linux
osgViewer::Viewer * mViewer;

//Scene transforms. mRootNode is used by our osgViewer. The others are children of mRootNode.
osg::ref_ptr<osg::Group> mRootNode;
osg::ref_ptr<osg::MatrixTransform> mRootTrans;
osg::ref_ptr<osg::MatrixTransform> mNavTrans;
osg::ref_ptr<osg::MatrixTransform> mSceneTrans;
osg::ref_ptr<osg::MatrixTransform> mWelcomeTrans;

//Player object to handle player position, hp, pilot and gunner transforms etc.
Player player;


osg::ref_ptr<osg::FrameStamp> mFrameStamp; //to sync osg animations across cluster


//callbacks
void myInitOGLFun();
void myPreSyncFun();
void myPostSyncPreDrawFun();
void myDrawFun();
void myEncodeFun();
void myDecodeFun();
void myCleanUpFun();
void keyCallback(int key, int action);

//other functions
void initOSG();
void setupLightSource();

//variables to share across cluster
sgct::SharedFloat curr_time(0.0);			//Current game time

sgct::SharedFloat playerPosX(0.0);	//Initialize player vector and quaternions. SGCT and SFS does
sgct::SharedFloat playerPosY(-50.0);	//not like shared arrays, so every value is saved individually.
sgct::SharedFloat playerPosZ(0.0);
sgct::SharedFloat baseQuatX(0.0);			//X = yaw, Y = roll, Z = pitch, W = angle. (quaternion)
sgct::SharedFloat baseQuatY(0.0);
sgct::SharedFloat baseQuatZ(0.0);
sgct::SharedFloat baseQuatW(0.0);
sgct::SharedFloat gunnerQuatX(0.0);
sgct::SharedFloat gunnerQuatY(0.0);
sgct::SharedFloat gunnerQuatZ(0.0);
sgct::SharedFloat gunnerQuatW(0.0);

sgct::SharedBool fireSync(false);
sgct::SharedInt randomSeed(0);

sgct::SharedBool wireframe(false);			//OsgExample settings
sgct::SharedBool info(false);
sgct::SharedBool stats(false);
sgct::SharedBool takeScreenshot(false);
sgct::SharedBool light(true);


//State of the game. 0 = Welcome Screen. 1 = Game Screen. 2 = Gameover Screen. 3 = Pre-game Screen.
//newState is used to let all nodes know which frame the state-change takes place. 
sgct::SharedInt gameState(0);
sgct::SharedBool newState(true);


//Variables for dome-client controls
bool Buttons[9];
enum directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, ROLLLEFT, ROLLRIGHT, SHOOT };

//These variables are currently set by "config/variables.txt". These values are only here as a reference for values that works.
float accRotVal = 0.005;
float accRotMax = 0.4;
float accThrustVal = 0.006;
float accThrustMax = 0.4;
float fireRate = 0.4;	//One bullet / 400ms
float projectileVelocity = 12000.0;
float shakeVal = 2.5;

float navigationSpeed = 0.0; // Current player speed
float accRotX = 0.0, accRotY = 0.0, accRotZ = 0.0;	//rotational acceleration
float rotX = 0.0,	 rotY = 0.0,	rotZ = 0.0;		//rotational velocity
float accThrust = 0.0;
float fireTimer = 0.0;
float shakeTime = 0.0;
int asteroidAmount = 1;
float lightval = 0.8;
bool domtest = false;

double gInputRotX = 0.0, gInputRotY = 0.0;
double pInputRotX = 0.0, pInputRotY = 0.0, pInputRotZ = 0.0;
double eInputEngine = 0.5, eInputShield = 0.5, eInputTurret = 0.5;

//Shake bridge on collision
bool shakeBridge = false;

//Global index for objects in scene
int objIndex = 0;


float demoTime = 5.0;

//(0, forward_dir)
osg::Quat baseQuat = osg::Quat(0, 0, 1, 0);
osg::Quat gunnerQuat = osg::Quat(0, 0, 0, 1);

//Convert player- and gunner direction to osg format. Only the master node uses these variables.
osg::Vec3f osg_forward_dir = osg::Vec3f(0, 1, 0);
osg::Vec3f osg_up_dir = osg::Vec3f(0, 0, 1);
osg::Vec3f gunner_up = osg::Vec3f(0, 0, 1);
osg::Vec3f gunner_side = osg::Vec3f(1, 0, 0);


//vector containing all projectiles in the scene
std::list<Projectile> missiles;

//vector containing all objects (asteroids, enemies, etc.) in the scene
std::list<GameObject*> objectList;


// Manage sound handling
SoundManager soundManager;

// Benchmarking static vars
double NetworkManager::start;
double NetworkManager::end;
int NetworkManager::itemsSent;
bool NetworkManager::benchmarkingStarted = false;



//! When something from a server extension is received this function is called. Could be position updating  of gameobject, a private message or just a notification. The ["cmd"] parameter of the event that is received  reveals which extension that was spitting out the info. Based on extension this function will do different things.
void NetworkManager::OnSmartFoxExtensionResponse(unsigned long long ptrContext, boost::shared_ptr<BaseEvent> ptrEvent) {
	// get pointer to main frame.
	NetworkManager* ptrMainFrame = (NetworkManager*)ptrContext;

	// Check that we're still alive and running
	if (ptrMainFrame == NULL) {
		return;
	}

	// Get the cmd parameter of the event
	boost::shared_ptr<map<string, boost::shared_ptr<void>>> ptrEventParams = ptrEvent->Params();
	boost::shared_ptr<void> ptrEventParamValueCmd = (*ptrEventParams)["cmd"];
	boost::shared_ptr<string> ptrNotifiedCmd = ((boost::static_pointer_cast<string>)(ptrEventParamValueCmd));


	// check the type of the command
	if (*ptrNotifiedCmd == "PilotEvent") {
		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		pInputRotX = *(ptrNotifiedISFSObject->GetDouble("sgctRotY"));
		pInputRotZ = *(ptrNotifiedISFSObject->GetDouble("sgctRotX"));

		bool pInputForward = *(ptrNotifiedISFSObject->GetBool("sgctForward"));
		//bool pInputBackward = *(ptrNotifiedISFSObject->GetBool("sgctBackward"));

		accRotX += pInputRotX * accRotVal;
		accRotZ += pInputRotZ * accRotVal;

		if (pInputForward && navigationSpeed < accThrustMax * eInputEngine) {
			navigationSpeed += accThrustVal * eInputEngine;
		}
		//if (pInputBackward && navigationSpeed > -0.1) {
		//	navigationSpeed -= accThrustVal * eInputEngine;
		//}
	}

	if (*ptrNotifiedCmd == "GunnerEvent") {

		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		gInputRotX = *(ptrNotifiedISFSObject->GetDouble("sgctRotX")) * 1.5;
		gInputRotY = *(ptrNotifiedISFSObject->GetDouble("sgctRotY")) * 1.5;
		bool fire = *(ptrNotifiedISFSObject->GetBool("sgctFire"));

		if (fire && fireTimer <= 0.0 ) {
			soundManager.play("laser", osg::Vec3f(0.0f, 0.0f, 0.0f));
			fireSync.setVal(true);
			fireTimer = fireRate / eInputTurret;
		}
	}
	if (*ptrNotifiedCmd == "EngineerEvent") {
		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		eInputEngine = ( (float)(*(ptrNotifiedISFSObject->GetFloat("sgctEngine"))) * 3 + 0.5);
		eInputShield = ( (float)(*(ptrNotifiedISFSObject->GetFloat("sgctShield"))) + 0.5);
		eInputTurret = ( (float)(*(ptrNotifiedISFSObject->GetFloat("sgctTurret"))) + 0.5);
		
	}
	if (*ptrNotifiedCmd == "BenchMarking") {

		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		double item = *(ptrNotifiedISFSObject->GetDouble("1"));
		double item2 = *(ptrNotifiedISFSObject->GetDouble("2"));
		double item3 = *(ptrNotifiedISFSObject->GetDouble("3"));
		double item4 = *(ptrNotifiedISFSObject->GetDouble("4"));

		end = omp_get_wtime();
		std::cout << "Reply from server, " << static_cast<int>((end - start) * 1000) << "ms." << endl;

		start = omp_get_wtime();
		// send new item 
		if (itemsSent++ < 35 && benchmarkingStarted) {
			boost::shared_ptr<ISFSObject> parameters(new SFSObject());

			parameters->PutDouble("1", 0.923);
			parameters->PutDouble("2", 0.953);
			parameters->PutDouble("3", 0.343);
			parameters->PutDouble("4", 0.523);

			// find our room to send to.
			boost::shared_ptr<Room> lastJoined = ptrMainFrame->m_ptrSmartFox->LastJoinedRoom();

			// Perform extensionrequest
			boost::shared_ptr<IRequest> extRequest(new ExtensionRequest("BenchMarking", parameters, lastJoined));
			ptrMainFrame->m_ptrSmartFox->Send(extRequest);
		}
		else {
			benchmarkingStarted = false;
			itemsSent = 0;
		}
	}
}

// Handle networking
NetworkManager manager;

int main(int argc, char* argv[])
{
	manager.init();
	soundManager.init();

	fstream freader;
	string trash;
	freader.open("Configuration/variables.txt");
	if (freader.is_open()) {
		freader >> trash >> accRotVal
				>> trash >> accRotMax
				>> trash >> accThrustVal
				>> trash >> accThrustMax
				>> trash >> fireRate
				>> trash >> projectileVelocity
				>> trash >> shakeVal
				>> trash >> asteroidAmount
				>> trash >> lightval
				>> trash >> domtest;
	}
	freader.close();

	//SGCT setup
	gEngine = new sgct::Engine(argc, argv);

	gEngine->setInitOGLFunction(myInitOGLFun);
	gEngine->setPreSyncFunction(myPreSyncFun);
	gEngine->setPostSyncPreDrawFunction(myPostSyncPreDrawFun);
	gEngine->setDrawFunction(myDrawFun);
	gEngine->setCleanUpFunction(myCleanUpFun);
	gEngine->setKeyboardCallbackFunction(keyCallback);


	//fix incompability with warping and OSG
	sgct_core::ClusterManager::instance()->setMeshImplementation(sgct_core::ClusterManager::DISPLAY_LIST);

	//Initialize buttoninput
	for (int i = 0; i<9; i++)
		Buttons[i] = false;

	if (!gEngine->init())
	{
		delete gEngine;
		return EXIT_FAILURE;
	}

	//Initialize cluster-shared variables
	sgct::SharedData::instance()->setEncodeFunction(myEncodeFun);
	sgct::SharedData::instance()->setDecodeFunction(myDecodeFun);

	// Main loop
	gEngine->render();

	// Clean up
	delete gEngine;

	// Exit program
	exit(EXIT_SUCCESS);
}


/*!
* SGCT - Initialize OpenGL
*/
void myInitOGLFun()
{
	//Setup OSG scene graph and viewer.
	initOSG();

	//Generate random seed. (0-1000)
	if (gEngine->isMaster()) {
		srand(time(NULL));
		randomSeed.setVal(round(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* 1000));
	}

	//Initialize scene graph transforms. mRootNode is initialized in the InitOSG() function
	mRootTrans = new osg::MatrixTransform();
	mNavTrans = new osg::MatrixTransform();
	mSceneTrans = new osg::MatrixTransform();
	mWelcomeTrans = new osg::MatrixTransform();

	
	//Setup the scene graph
	mRootNode->addChild(mRootTrans);
	mRootTrans->addChild(mNavTrans);
	mRootTrans->addChild(mWelcomeTrans);
	mNavTrans->addChild(mSceneTrans);

	//Transform to scene transformation from configuration file and apply static rotation to compensate for OSG-SGCT
	mRootTrans->setMatrix(osg::Matrix(glm::value_ptr(gEngine->getModelMatrix())));
	mRootTrans->postMult(osg::Matrix::rotate(-PI / 2.0, 1.0, 0.0, 0.0));


	//Setup the lightsource
	setupLightSource();
}
float x = 0;
double checkIfImAwake = 100;

void myPreSyncFun()
{
	//The master node handle calculation of new values that will later be synced with all nodes
	if (gEngine->isMaster())
	{
		//Update current time
		curr_time.setVal(sgct::Engine::getTime());

		if (checkIfImAwake < curr_time.getVal()) {
			manager.alarm();
			checkIfImAwake += 100;
		}

		switch (gameState.getVal()) {
			//Welcome Screen
			case 0: {

			}
			break;

			//Pre-game Screen
			case 3: {
			
			}
			break;

			//Game Screen
			case 1: {

				// Update velocities based on SGCT key input 
				if (Buttons[FORWARD] && navigationSpeed < accThrustMax)
					navigationSpeed += accThrustVal;
				if (Buttons[BACKWARD] && navigationSpeed > -accThrustMax)
					navigationSpeed -= accThrustVal;
				if (Buttons[LEFT])
					accRotZ -= accRotVal;
				if (Buttons[RIGHT])
					accRotZ += accRotVal;
				if (Buttons[DOWN])
					accRotX += accRotVal;
				if (Buttons[UP])
					accRotX -= accRotVal;
				if (Buttons[ROLLRIGHT])
					accRotY -= accRotVal;
				if (Buttons[ROLLLEFT])
					accRotY += accRotVal;
				if (Buttons[SHOOT] && fireTimer <= 0.0){
					//manager.startBenchmarking();
					fireSync.setVal(true);
					fireTimer = fireRate;
				}


				//Cooldown time for the gunner laser
				if (fireTimer > 0.0)
					fireTimer -= gEngine->getDt();


				//Update pilot values. Energy loss is used for retardation of the ship.
				navigationSpeed = navigationSpeed * 0.97; 
				accRotX = accRotX*0.97;
				accRotY = accRotY*0.97;
				accRotZ = accRotZ*0.97;
				gInputRotX = gInputRotX * 0.70;
				gInputRotY = gInputRotY * 0.70;

				//Get new forward direction and player position.
				//Transformations are inversed because world moves relative to the camera.
				osg_forward_dir = baseQuat * osg::Vec3f(0, -1, 0);
				osg_up_dir = baseQuat * osg::Vec3f(0, 0, -1);
				player.setPos(player.getPos() - osg_forward_dir * navigationSpeed);	//Minus for inverse navigation

				gunner_side = gunnerQuat * osg::Vec3f(1, 0, 0);
				gunner_up = gunnerQuat * osg::Vec3f(0, 0, 1);


				//Set direction and position of player
				rotX = accRotX * gEngine->getDt();
				rotY = accRotY * gEngine->getDt();
				rotZ = accRotZ * gEngine->getDt();

				//Get player local coordinate system
				osg::Vec3f normal1 = osg_forward_dir;
				osg::Vec3f normal2 = osg_up_dir;
				osg::Vec3f normal3 = normal2^normal1; //cross product

				osg::Quat rX = osg::Quat(rotX, normal3);
				osg::Quat rY = osg::Quat(rotY, normal1);
				osg::Quat rZ = osg::Quat(-rotZ, normal2);

				//Get rotated quaternion
				baseQuat = baseQuat * rX.conj();
				baseQuat = baseQuat * rY.conj();
				baseQuat = baseQuat * rZ.conj();


				//Set direction for the gunner crosshair.
				//External input uses X- and Y coordinates which is translated to X- and Z in SGCT
				osg::Quat gRX = osg::Quat(gInputRotX * gEngine->getDt(), gunner_up);
				osg::Quat gRZ = osg::Quat(-gInputRotY * gEngine->getDt(), gunner_side);

				osg::Vec3f tempVec = gunnerQuat * osg::Vec3f(0, 1, 0);
				float angle = acos(tempVec*osg::Vec3f(0, 1, 0));

				if (domtest) {
					//Limit gunner crosshair to 1.2 radians from center and to positive z
					if (angle > 1.2) {
						osg::Vec3f tempVec2 = tempVec^osg::Vec3f(0, 1, 0);
						gunnerQuat = gunnerQuat * osg::Quat((sqrt(pow(gInputRotX, 2) + pow(gInputRotY, 2))) * gEngine->getDt(), tempVec2);
					}
					gunnerQuat = gunnerQuat * gRX.conj();

					if (tempVec.z() > 0.0) {
						gunnerQuat = gunnerQuat * gRZ.conj();
					}
					else
						gunnerQuat = gunnerQuat * osg::Quat(0.05 * gEngine->getDt(), osg::Vec3(1, 0, 0));
				}
				else {
					gunnerQuat = gunnerQuat * gRZ.conj();
					gunnerQuat = gunnerQuat * gRX.conj();
				}

				//Sync values with all nodes
				playerPosX.setVal(player.getPos().x());
				playerPosY.setVal(player.getPos().y());
				playerPosZ.setVal(player.getPos().z());
				baseQuatX.setVal(baseQuat.x());
				baseQuatY.setVal(baseQuat.y());
				baseQuatZ.setVal(baseQuat.z());
				baseQuatW.setVal(baseQuat.w());
				gunnerQuatX.setVal(gunnerQuat.x());
				gunnerQuatY.setVal(gunnerQuat.y());
				gunnerQuatZ.setVal(gunnerQuat.z());
				gunnerQuatW.setVal(gunnerQuat.w());
			}
			break;

		}
	}
}

void myPostSyncPreDrawFun()
{
	//setGameState is called for all nodes if newState is true
	if (newState.getVal()) {
		setGameState(gameState.getVal(), objIndex, objectList, player, mNavTrans, mRootTrans, mSceneTrans, mWelcomeTrans, soundManager, randomSeed.getVal(), asteroidAmount);
		newState.setVal(false);
	}
	

	switch (gameState.getVal()) {
	//Welcome Screen
		case 0: {
			// DO NOTHING FOR NOW. MAYBE SHOW BILLBOARD 
			mNavTrans->postMult(osg::Matrix::rotate(0.03*gEngine->getDt(), -1.0, 0.1, 0.1));
		}
		break;

	//Game Screen
		case 1: {

			//Demotest. Spawn enemies every 3 second.
			if (demoTime < 0.0)
			{
				int rand1 = 5000 - (randomSeed.getVal() * 3571 + 997) % 1000;
				int rand2 = 5000 - ((5000 + rand1) * 3571 + 997) % 10000;
				int rand3 = 5000 - ((5000 + rand2) * 3571 + 997) % 10000;
				randomSeed.setVal(5000 + rand3);
				objectList.push_back(new EnemyShip((std::string)("Enemy"), player.getPos() + osg::Vec3f((float)rand1, (float)rand2, (float)rand3), 250.0f, (std::string)("models/fiendeskepp.ive"), mSceneTrans, 3, objIndex++));
				demoTime = 10.0;
			}
			demoTime -= gEngine->getDt();

			if (fireSync.getVal())
			{
				//Add and then sort new projectiles in the missile vector.
				osg::Vec3f tempVec = (gunnerQuat * baseQuat) * osg::Vec3f(0, 1, 0);
				osg::Quat tempDir = gunnerQuat * baseQuat;
				missiles.push_back(Projectile((std::string)("Laser"), player.getPos() + baseQuat * osg::Vec3f(200.0, 0.0, -100.0), tempVec, tempDir, (std::string)("models/skott20m.obj"), mSceneTrans, 1.0f, navigationSpeed + projectileVelocity));
				missiles.push_back(Projectile((std::string)("Laser"), player.getPos() + baseQuat * osg::Vec3f(-200.0, 0.0, -100.0), tempVec, tempDir, (std::string)("models/skott20m.obj"), mSceneTrans, 1.0f, navigationSpeed + projectileVelocity));
				fireSync.setVal(false);
			}

			//Check OsgExample settings
			gEngine->setWireframe(wireframe.getVal());
			gEngine->setDisplayInfoVisibility(info.getVal());
			gEngine->setStatsGraphVisibility(stats.getVal());

			//Enable or disable light with the L button
			light.getVal() ? mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE) :
				mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);


			//Sync updated quaternions and vector
			baseQuat = osg::Quat(baseQuatX.getVal(), baseQuatY.getVal(), baseQuatZ.getVal(), baseQuatW.getVal());
			gunnerQuat = osg::Quat(gunnerQuatX.getVal(), gunnerQuatY.getVal(), gunnerQuatZ.getVal(), gunnerQuatW.getVal());
			player.setPos(osg::Vec3f(playerPosX.getVal(), playerPosY.getVal(), playerPosZ.getVal()));


		//Perform matrix transformations

			//Player translation and rotation
			mNavTrans->setMatrix(osg::Matrix::identity());

			mNavTrans->postMult(osg::Matrix::rotate(baseQuat));
			mNavTrans->postMult(osg::Matrix::translate(player.getPos()));
			mNavTrans->setMatrix(mNavTrans->getInverseMatrix());
			
			//Gunner rotation
			player.rotateGunnerTrans(gunnerQuat);

			
		//Collision handling. This should probably get moved into its own class

			//Move, remove and check collisions for missiles.
			for (list<Projectile>::iterator mIterator = missiles.begin(); mIterator != missiles.end(); mIterator++)
			{
				//Reduce lifetime of missile
				mIterator->setLifeTime(mIterator->getLifeTime() - gEngine->getDt());
				if (mIterator->getLifeTime() < 0.0f)
				{
					//Remove from scene graph before deleting the object
					mIterator->removeChildModel(mIterator->getModel());
					missiles.erase(mIterator);
					goto stop;
				}
				else
				{
					mIterator->translate(mIterator->getDir()*mIterator->getVel()*gEngine->getDt());


					//Collision check with player
					if ((mIterator->getPos() - player.getPos()).length() < mIterator->getColRad() + player.getColRad())
					{
						mIterator->removeChildModel(mIterator->getModel());
						missiles.erase(mIterator);
						shakeTime = 0.5;
						goto stop;		//break all current loops. stop is located directly after the collision handling loops.
					}

					//Collision check with objects in the scene
					for (list<GameObject*>::iterator oIterator = objectList.begin(); oIterator != objectList.end(); oIterator++)
					{
						if ((mIterator->getPos() - (*oIterator)->getPos()).length() < mIterator->getColRad() + (*oIterator)->getColRad())
						{
							mIterator->removeChildModel(mIterator->getModel());
							(*oIterator)->removeChildModel((*oIterator)->getModel());
							

							if (domtest) createExplosion(500.0, (*oIterator)->getPos() - player.getPos(), "textures/dome_startscreen.png", mSceneTrans, 3.0, 3.0);
							soundManager.play("explosion", player.getPos()  - (*oIterator)->getPos());

							missiles.erase(mIterator);
							objectList.erase(oIterator);

							goto stop;
						}
					}
				}
			}

			//Check collision with player and update enemy AI
			for (list<GameObject*>::iterator oIterator = objectList.begin(); oIterator != objectList.end(); oIterator++)
			{
				if ((player.getPos() - (*oIterator)->getPos()).length() < player.getColRad() + (*oIterator)->getColRad())
				{
					//createExplosion(1.0, player.getPos() - (*oIterator)->getPos(), "textures/dome_startscreen.png", mSceneTrans, 3.0, 3.0);
					soundManager.play("explosion", osg::Vec3f(0.0f, 0.0f, 0.0f));
					(*oIterator)->removeChildModel((*oIterator)->getModel());
					//delete (*oIterator);
					objectList.erase(oIterator);
					shakeTime = 0.5;
					goto stop;
				}

				//Collision check object-object
				for (list<GameObject*>::iterator oIterator2 = objectList.begin(); oIterator2 != objectList.end(); oIterator2++)
				{
					if (oIterator != oIterator2 && ((*oIterator)->getPos() - (*oIterator2)->getPos()).length() < (*oIterator)->getColRad() + (*oIterator2)->getColRad())
					{
						(*oIterator)->removeChildModel((*oIterator)->getModel());
						(*oIterator2)->removeChildModel((*oIterator2)->getModel());

						//createExplosion(500.0, player.getPos() - (*oIterator)->getPos(), "textures/dome_startscreen.png", mSceneTrans, 3.0, 3.0);
						soundManager.play("explosion", player.getPos() - (*oIterator)->getPos());

						objectList.erase(oIterator);
						objectList.erase(oIterator2);

						goto stop;
					}
				}

				//Update AI-related stuff
				if ((*oIterator)->getName() == "Enemy")
				{
					(*oIterator)->updateAI(player.getPos(), missiles, mSceneTrans, gEngine->getDt());
				}

			}
			stop: //this is where the "goto stop" command goes



			//Shake camera on player collision with objects
			if (shakeTime > 0.0) {
				//Because the values need to be synced, we can't use standard random values since they depend on a local random seed.
				//Modulus operator retuns an int, so we need to re-cast it as a float.

				float rand1 = (float)(randomSeed.getVal() % 911) / 911; //generate random value between 0 and 1
				float rand2 = (float)((117*randomSeed.getVal()) % 911) / 911; //generate new random value between 0 and 1

				//Generate new random seed
				randomSeed.setVal(round(static_cast <float> (rand()) / static_cast <float> (RAND_MAX)* 1000));

				mNavTrans->preMult(osg::Matrix::rotate(shakeVal*gEngine->getDt(), rand1, rand2, 0));
				shakeTime -= gEngine->getDt();
			}
		}
		break;
	
	//Pre-game Screen
		case 3: {
					if (curr_time.getVal() > 38){
						gameState.setVal(0); //Welcome Screen
						newState.setVal(true);
					}

		}
		break;
	}


	//update the frame stamp in the viewer to sync all
	//time based events in osg
	mFrameStamp->setFrameNumber(gEngine->getCurrentFrameNumber());
	mFrameStamp->setReferenceTime(curr_time.getVal());
	mFrameStamp->setSimulationTime(curr_time.getVal());
	mViewer->setFrameStamp(mFrameStamp.get());
	mViewer->advance(curr_time.getVal()); //update
	//mViewer->setCameraManipulator(nodeTracker.get());//cookbook

	//traverse if there are any tasks to do
	if (!mViewer->done())
	{
		mViewer->eventTraversal();
		//update travelsal needed for pagelod object like terrain data etc.
		mViewer->updateTraversal();
	}
}

void myDrawFun()
{
	glLineWidth(2.0f);

	gEngine->setNearAndFarClippingPlanes(0.1f, 100000.0f);
	const int * curr_vp = gEngine->getActiveViewportPixelCoords();
	mViewer->getCamera()->setViewport(curr_vp[0], curr_vp[1], curr_vp[2], curr_vp[3]);
	mViewer->getCamera()->setProjectionMatrix(osg::Matrix(glm::value_ptr(gEngine->getActiveViewProjectionMatrix())));

	mViewer->renderingTraversals();
}

void myEncodeFun()
{
	sgct::SharedData::instance()->writeFloat(&curr_time);
	sgct::SharedData::instance()->writeFloat(&playerPosX);
	sgct::SharedData::instance()->writeFloat(&playerPosY);
	sgct::SharedData::instance()->writeFloat(&playerPosZ);
	sgct::SharedData::instance()->writeFloat(&baseQuatX);
	sgct::SharedData::instance()->writeFloat(&baseQuatY);
	sgct::SharedData::instance()->writeFloat(&baseQuatZ);
	sgct::SharedData::instance()->writeFloat(&baseQuatW);
	sgct::SharedData::instance()->writeFloat(&gunnerQuatX);
	sgct::SharedData::instance()->writeFloat(&gunnerQuatY);
	sgct::SharedData::instance()->writeFloat(&gunnerQuatZ);
	sgct::SharedData::instance()->writeFloat(&gunnerQuatW);
	sgct::SharedData::instance()->writeBool(&fireSync);
	sgct::SharedData::instance()->writeInt(&gameState);
	sgct::SharedData::instance()->writeBool(&newState);
	sgct::SharedData::instance()->writeInt(&randomSeed);
	sgct::SharedData::instance()->writeBool(&wireframe);
	sgct::SharedData::instance()->writeBool(&info);
	sgct::SharedData::instance()->writeBool(&stats);
	sgct::SharedData::instance()->writeBool(&takeScreenshot);
	sgct::SharedData::instance()->writeBool(&light);
}

void myDecodeFun()
{
	sgct::SharedData::instance()->readFloat(&curr_time);
	sgct::SharedData::instance()->readFloat(&playerPosX);
	sgct::SharedData::instance()->readFloat(&playerPosY);
	sgct::SharedData::instance()->readFloat(&playerPosZ);
	sgct::SharedData::instance()->readFloat(&baseQuatX);
	sgct::SharedData::instance()->readFloat(&baseQuatY);
	sgct::SharedData::instance()->readFloat(&baseQuatZ);
	sgct::SharedData::instance()->readFloat(&baseQuatW);
	sgct::SharedData::instance()->readFloat(&gunnerQuatX);
	sgct::SharedData::instance()->readFloat(&gunnerQuatY);
	sgct::SharedData::instance()->readFloat(&gunnerQuatZ);
	sgct::SharedData::instance()->readFloat(&gunnerQuatW);
	sgct::SharedData::instance()->readBool(&fireSync);
	sgct::SharedData::instance()->readInt(&gameState);
	sgct::SharedData::instance()->readBool(&newState);
	sgct::SharedData::instance()->readInt(&randomSeed);
	sgct::SharedData::instance()->readBool(&wireframe);
	sgct::SharedData::instance()->readBool(&info);
	sgct::SharedData::instance()->readBool(&stats);
	sgct::SharedData::instance()->readBool(&takeScreenshot);
	sgct::SharedData::instance()->readBool(&light);
}

void myCleanUpFun()
{
	sgct::MessageHandler::instance()->print("Cleaning up osg data...\n");
	delete mViewer;
	mViewer = NULL;
}

//SGCT key callbacks that will be replaced after merge with SFS
void keyCallback(int key, int action)
{
	//newState is used to sync changes in gamestate between nodes

	if (gEngine->isMaster())
	{
		if (gameState == 1) {
			switch (key)
			{
			case 'K':
				if (action == SGCT_PRESS)
					stats.toggle();
				break;

			case 'I':
				if (action == SGCT_PRESS)
					info.toggle();
				break;

			case 'L':
				if (action == SGCT_PRESS)
					light.toggle();
				break;

			case 'F':
				if (action == SGCT_PRESS)
					wireframe.toggle();
				break;

			case 'T':
				if (action == SGCT_PRESS)
					gEngine->terminate();
				break;

			case 'P':
			case SGCT_KEY_F10:
				if (action == SGCT_PRESS)
					takeScreenshot.setVal(true);
				break;

			case 'Q':
				Buttons[ROLLLEFT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;

			case 'E':
				Buttons[ROLLRIGHT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'D':
				Buttons[RIGHT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'A':
				Buttons[LEFT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'W':
				Buttons[UP] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'S':
				Buttons[DOWN] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'Z':
				Buttons[FORWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case 'X':
				Buttons[BACKWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			case SGCT_KEY_SPACE:
				Buttons[SHOOT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
				break;
			}
		}
		else {
			if (key == 'X') {
				gameState.setVal(1); //Start game
				newState.setVal(true);
			}
		}
	}
}

void initOSG()
{
	mRootNode = new osg::Group();
	osg::Referenced::setThreadSafeReferenceCounting(true);

	// Create the osgViewer instance
	mViewer = new osgViewer::Viewer;

	// Create a time stamp instance
	mFrameStamp = new osg::FrameStamp();

	//run single threaded when embedded
	mViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);

	// Set up osgViewer::GraphicsWindowEmbedded for this context
	osg::ref_ptr< ::osg::GraphicsContext::Traits > traits =
		new osg::GraphicsContext::Traits;

	osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow =
		new osgViewer::GraphicsWindowEmbedded(traits.get());

	mViewer->getCamera()->setGraphicsContext(graphicsWindow.get());

	//SGCT will handle the near and far planes
	mViewer->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
	mViewer->getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

	//disable osg from clearing the buffers that will be done by SGCT
	GLbitfield tmpMask = mViewer->getCamera()->getClearMask();
	mViewer->getCamera()->setClearMask(tmpMask & (~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)));

	mViewer->setSceneData(mRootNode.get());
}

void setupLightSource()
{
	osg::Light *light0 = new osg::Light;
	osg::Light *light1 = new osg::Light;
	osg::LightSource* lightSource0 = new osg::LightSource;
	osg::LightSource* lightSource1 = new osg::LightSource;

	light0->setLightNum(0);
	light0->setPosition(osg::Vec4(80000.0f, 0.0f, 0.0f, 1.0f));
	light0->setAmbient(osg::Vec4(lightval, lightval, lightval, 1.0f));
	light0->setDiffuse(osg::Vec4(2*lightval, 2*lightval, 2*lightval, 1.0f));
	light0->setSpecular(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	light0->setConstantAttenuation(1.0f);

	lightSource0->setLight(light0);	//Static light
	lightSource0->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource0->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	light1->setLightNum(1);	//Moves with player
	light1->setPosition(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	light1->setAmbient(osg::Vec4(0.4f, 0.4f, 0.4f, 1.0f));
	light1->setDiffuse(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	light1->setSpecular(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	light1->setConstantAttenuation(0.05f);
	light1->setLinearAttenuation(0.05f);

	lightSource1->setLight(light1);
	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource1->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	mNavTrans->addChild(lightSource0);
	mRootNode->addChild(lightSource1);
}
