/*
osgExample_sfs
*/

#include "sgct.h"

#include<string.h>
#include<stdlib.h>
#include<time.h>
#include <fstream>

/* Custom items */
#include "classroom\SoundManager.h"

#include "classroom\SkyBox.h"
#include "classroom\Projectile.h"
#include "classroom\EnemyShip.h"
#include "classroom\NetworkManager.h"
#include "classroom\Billboard.h"



sgct::Engine * gEngine;

//Not using ref pointers enables
//more controlled termination
//and prevents segfault on Linux
osgViewer::Viewer * mViewer;

//Scene transforms. mRootNode is used by our osgViewer
osg::ref_ptr<osg::Group> mRootNode;
osg::ref_ptr<osg::MatrixTransform> mNavTrans;
osg::ref_ptr<osg::MatrixTransform> mSceneTrans;
osg::ref_ptr<osg::MatrixTransform> mPlayerTrans;
osg::ref_ptr<osg::MatrixTransform> mGunnerTrans;
osg::ref_ptr<osg::MatrixTransform> mBridgeTrans;

//Position and direction variables for the player
osg::Vec3f player_pos = osg::Vec3f(0,0,0);
//(0, forward_dir)
osg::Quat baseQuat = osg::Quat(0, 0, 1, 0);
osg::Quat gunnerQuat = osg::Quat(0, 1, 0, 0);

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
sgct::SharedFloat forward_dir_x(0.0);	//Initialize player direction. SGCT and SFS does
sgct::SharedFloat forward_dir_y(1.0);	//not seem to like shared arrays, so every value is saved individually
sgct::SharedFloat forward_dir_z(0.0);
sgct::SharedFloat up_dir_x(0.0);			
sgct::SharedFloat up_dir_y(0.0);
sgct::SharedFloat up_dir_z(1.0);
sgct::SharedFloat navigation_speed(0.0);	//Current player speed
sgct::SharedFloat rotX(0.0);				//rotX = yaw, rotY = roll, rotZ = pitch
sgct::SharedFloat rotY(0.0);
sgct::SharedFloat rotZ(0.0);
sgct::SharedBool fireSync(false);
sgct::SharedBool wireframe(false);			//OsgExample settings
sgct::SharedBool info(false);
sgct::SharedBool stats(false);
sgct::SharedBool takeScreenshot(false);
sgct::SharedBool light(true);
sgct::SharedDouble gInputRotX(0.0);
sgct::SharedDouble gInputRotY(0.0);
sgct::SharedDouble pInputRotX(0.0);
sgct::SharedDouble pInputRotY(0.0);
sgct::SharedDouble pInputRotZ(0.0);
sgct::SharedDouble eInputEngine(0.0);
sgct::SharedDouble eInputShield(0.0);
sgct::SharedDouble eInputTurret(0.0);
sgct::SharedFloat shakeTime(0.0);

//Temporary variables that will be replaced after merge with SFS
bool Buttons[9];
enum directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, ROLLLEFT, ROLLRIGHT, SHOOT };

float accRotVal = 0.005;
float accRotMax = 0.4;
float accThrustVal = 0.006;
float accThrustMax = 0.4;
float accRotX = 0.0;
float accRotY = 0.0;
float accRotZ = 0.0;
float accThrust = 0.0;


//Convert player direction to osg format
osg::Vec3f osg_forward_dir = osg::Vec3f(forward_dir_x.getVal(), forward_dir_y.getVal(), forward_dir_z.getVal());
osg::Vec3f osg_up_dir = osg::Vec3f(up_dir_x.getVal(), up_dir_y.getVal(), up_dir_z.getVal());

//vector containing all projectiles in the scene
std::list<Projectile> missiles;

//vector containg all objects (asteroids) in the scene
std::list<GameObject> objectList;

GameObject player;
GameObject bridge;

//Shake bridge on collision
bool shakeBridge = false;

//Global index for objects in scene
int objIndex = 0;

float fireRate = 0.4; //One bullet / 400ms
float projectileVelocity = 20.0;
float fireTimer = 0.0;

// Manage sound handling
SoundManager soundManager;

// Benchmarking static vars
double NetworkManager::start;
double NetworkManager::end;
int NetworkManager::itemsSent;
bool NetworkManager::benchmarkingStarted = false;

// Game logic

enum GameState { WELCOME_SCREEN, GAME_SCREEN, GAMEOVER_SCREEN };
void setGameState(GameState _state);

GameState state;


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

		pInputRotX.setVal(*(ptrNotifiedISFSObject->GetDouble("sgctRotY")));
		pInputRotZ.setVal(*(ptrNotifiedISFSObject->GetDouble("sgctRotX")));

		bool pInputForward = *(ptrNotifiedISFSObject->GetBool("sgctForward"));
		bool pInputBackward = *(ptrNotifiedISFSObject->GetBool("sgctBackward"));

		accRotX += pInputRotX * accRotVal;
		accRotZ += pInputRotZ * accRotVal;

		if (pInputForward && navigation_speed.getVal() < accThrustMax) {
			navigation_speed.setVal(navigation_speed.getVal()+accThrustVal);
		}
		if (pInputBackward && navigation_speed.getVal() > -0.1) {
			navigation_speed.setVal(navigation_speed.getVal() - accThrustVal);
		}
	}

	if (*ptrNotifiedCmd == "GunnerEvent") {

		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		gInputRotX.setVal(*(ptrNotifiedISFSObject->GetDouble("sgctRotX")));
		gInputRotY.setVal(*(ptrNotifiedISFSObject->GetDouble("sgctRotY")));
		bool fire = *(ptrNotifiedISFSObject->GetBool("sgctFire"));

		if (fire && fireTimer <= 0.0 ) {
			soundManager.play("laser", glm::vec3(0.0f, 0.0f, 0.0f));
			fireSync.setVal(true);
			fireTimer = fireRate;
		}
	}
	if (*ptrNotifiedCmd == "EngineerEvent") {
		boost::shared_ptr<void> ptrEventParamValueParams = (*ptrEventParams)["params"];
		boost::shared_ptr<ISFSObject> ptrNotifiedISFSObject = ((boost::static_pointer_cast<ISFSObject>)(ptrEventParamValueParams));

		eInputEngine.setVal(*(ptrNotifiedISFSObject->GetFloat("sgctEngine")));
		eInputShield.setVal(*(ptrNotifiedISFSObject->GetFloat("sgctShield")));
		eInputTurret.setVal(*(ptrNotifiedISFSObject->GetFloat("sgctTurret")));

		cout << "Engine: " << eInputEngine.getVal() << " Shield: " << eInputShield.getVal() << " Turret: " << eInputTurret.getVal() << endl;
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
				>> trash >> projectileVelocity;
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
	for (int i = 0; i<7; i++)
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
	//mViewer->setSceneData(createModel(mGunnerTrans, mRootNode)); //Create crosshair. Function declaration in classroom/Billboard.h
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

	//Generate random seed
	srand(time(NULL));

	//Skybox code needs to be cleaned up
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 90)));  //scene->getBound().radius())));
	geode->setCullingActive(false);
	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	skybox->setEnvironmentMap(0,
		osgDB::readImageFile("textures/skybox_biggerRT.png"), osgDB::readImageFile("textures/skybox_biggerLF.png"),
		osgDB::readImageFile("textures/skybox_biggerDN.png"), osgDB::readImageFile("textures/skybox_biggerUP.png"),
		osgDB::readImageFile("textures/skybox_biggerFT.png"), osgDB::readImageFile("textures/skybox_biggerBK.png"));
	skybox->addChild(geode.get());

	//Initialize scene graph transforms. mRootNode is initialized in the InitOSG() function
	mNavTrans = new osg::MatrixTransform();
	mSceneTrans = new osg::MatrixTransform();
	mPlayerTrans = new osg::MatrixTransform();
	mGunnerTrans = new osg::MatrixTransform();
	mBridgeTrans = new osg::MatrixTransform();
	mPlayerTrans->setMatrix(osg::Matrix::identity());
	mBridgeTrans->setMatrix(osg::Matrix::identity());

	//Setup the scene graph
	mRootNode->addChild(mPlayerTrans);
	mRootNode->addChild(mNavTrans);
	mNavTrans->addChild(mSceneTrans);
	mNavTrans->addChild(skybox);
	mPlayerTrans->addChild(mGunnerTrans);
	mPlayerTrans->addChild(mBridgeTrans);

	//Apply static rotation to compensate for OSG-SGCT and to transform commandbridge correctly
	mPlayerTrans->postMult(osg::Matrix::rotate(-PI / 2.0, 1.0, 0.0, 0.0));
	mBridgeTrans->postMult(osg::Matrix::rotate(PI / 4.0, 1.0, 0.0, 0.0));
	//mBridgeTrans->postMult(osg::Matrix::rotate(PI / 2.0, 0.0, 1.0, 0.0));
	mBridgeTrans->postMult(osg::Matrix::scale(0.1, 0.1, 0.1));

	//Setup the lightsource
	setupLightSource();

	soundManager.play("gameOver", glm::vec3(0.0f, 0.0f, 0.0f));
}

void myPreSyncFun()
{
	if (gEngine->isMaster())
	{
		switch (state) {
			case GAME_SCREEN: {
				//Update current time
				curr_time.setVal(sgct::Engine::getTime());

				if (fireTimer > 0.0)
					fireTimer -= gEngine->getDt();

				//Update pilot values
				accRotX = accRotX*0.97;
				accRotY = accRotY*0.97;
				accRotZ = accRotZ*0.97;
				navigation_speed.setVal((navigation_speed.getVal()*0.90));

				rotX.setVal(rotX.getVal() + (accRotX * gEngine->getDt()));
				rotY.setVal(rotY.getVal() + (accRotY * gEngine->getDt()));
				rotZ.setVal(rotZ.getVal() + (accRotZ * gEngine->getDt()));

				gInputRotX.setVal(gInputRotX.getVal()*0.90);
				gInputRotY.setVal(gInputRotY.getVal()*0.90);

				//cout << accRotX << endl;


				// Update velocities based on SGCT key input 
				if (Buttons[FORWARD] && navigation_speed.getVal() < 0.4)
					navigation_speed.setVal(navigation_speed.getVal() + 0.04);
				if (Buttons[BACKWARD] && navigation_speed.getVal() > -0.4)
					navigation_speed.setVal(navigation_speed.getVal() - 0.04);
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
			}
			break;

			case WELCOME_SCREEN: {


			}
			
			break;
		}
	}
}

void myPostSyncPreDrawFun()
{
	switch (state) {
		case GAME_SCREEN: {
			if (fireSync.getVal())
			{
				//Add and then sort new projectiles in the missile vector.
				osg::Quat tempQuat = gunnerQuat;
				tempQuat.x() = -tempQuat.x();
				osg::Vec3f tempVec = (tempQuat * baseQuat) * osg::Vec3f(0, 1, 0);
				osg::Quat tempDir = tempQuat * baseQuat;
				missiles.push_back(Projectile((std::string)("ettskott"), player_pos + baseQuat * osg::Vec3f(2, 0, 1), -tempVec, tempDir, (std::string)("models/skott.ive"), mSceneTrans, 1.0f, projectileVelocity));
				missiles.push_back(Projectile((std::string)("ettskott"), player_pos + baseQuat * osg::Vec3f(-2, 0, 1), -tempVec, tempDir, (std::string)("models/skott.ive"), mSceneTrans, 1.0f, projectileVelocity));

				fireSync.setVal(false);
			}

			//Check OsgExample settings
			gEngine->setWireframe(wireframe.getVal());
			gEngine->setDisplayInfoVisibility(info.getVal());
			gEngine->setStatsGraphVisibility(stats.getVal());

			//Enable or disable light with the L button
			light.getVal() ? mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE) :
				mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

			//Reset navigation transform every frame
			mNavTrans->setMatrix(osg::Matrix::identity());

			//
			//mNavTrans->postMult(osg::Matrix::scale(osg::Vec3f(0.1f, 0.1f, 0.1f)));

			//Rotate osg coordinate system to match sgct
			mNavTrans->preMult(osg::Matrix::rotate(-PI / 2, 1.0f, 0.0f, 0.0f));

			//Apply static translation to center camera position
			mNavTrans->preMult(osg::Matrix::translate(0.0f, 0.0f, -4.0f));


			//Set direction and position of player

			osg::Vec3f normal1 = osg::Vec3f(forward_dir_x.getVal(), forward_dir_y.getVal(), forward_dir_z.getVal());
			osg::Vec3f normal2 = osg::Vec3f(up_dir_x.getVal(), up_dir_y.getVal(), up_dir_z.getVal());
			osg::Vec3f normal3 = normal2^normal1;

			osg::Quat rX = osg::Quat(rotX.getVal(), normal3);
			osg::Quat rY = osg::Quat(rotY.getVal(), normal1);
			osg::Quat rZ = osg::Quat(rotZ.getVal(), normal2);


			//Get rotated quaternion
			baseQuat = baseQuat * rX.conj();
			baseQuat = baseQuat * rY.conj();
			baseQuat = baseQuat * rZ.conj();

			mNavTrans->postMult(osg::Matrix::rotate(baseQuat));
			mNavTrans->postMult(osg::Matrix::translate(player_pos));
			mNavTrans->setMatrix(mNavTrans->getInverseMatrix());


			//Gunner rotation
			mGunnerTrans->setMatrix(osg::Matrix::identity());
			//mGunnerTrans->preMult(osg::Matrix::rotate(-PI / 2, 1.0f, 0.0f, 0.0f));

			osg::Quat gRX = osg::Quat(gInputRotX.getVal() * gEngine->getDt(), osg::Vec3(0, 0, 1));
			osg::Quat gRZ = osg::Quat(-gInputRotY.getVal() * gEngine->getDt(), osg::Vec3(1, 0, 0));

			gunnerQuat = gunnerQuat * gRX.conj();
			gunnerQuat = gunnerQuat * gRZ.conj();

			mGunnerTrans->postMult(osg::Matrix::rotate(gunnerQuat));
			//mGunnerTrans->postMult(osg::Matrix::rotate(sin(gEngine->getDt()), 0, 0, 1));


			//Transform to scene transformation from configuration file
			mSceneTrans->setMatrix(osg::Matrix(glm::value_ptr(gEngine->getModelMatrix())));

			//Update forward and up direction
			osg_forward_dir = osg::Vec3f(forward_dir_x.getVal(), forward_dir_y.getVal(), forward_dir_z.getVal());
			osg_up_dir = osg::Vec3f(up_dir_x.getVal(), up_dir_y.getVal(), up_dir_z.getVal());

			osg_forward_dir = baseQuat * osg::Vec3f(0, -1, 0);
			osg_up_dir = baseQuat * osg::Vec3f(0, 0, -1);

			forward_dir_x.setVal(osg_forward_dir[0]);
			forward_dir_y.setVal(osg_forward_dir[1]);
			forward_dir_z.setVal(osg_forward_dir[2]);
			up_dir_x.setVal(osg_up_dir[0]);
			up_dir_y.setVal(osg_up_dir[1]);
			up_dir_z.setVal(osg_up_dir[2]);

			player_pos = player_pos + osg_forward_dir*navigation_speed.getVal();

			rotX.setVal(0.0);
			rotY.setVal(0.0);
			rotZ.setVal(0.0);


			//Move, remove and check collisions for missiles.
			//for (int i = 0; i < missiles.size(); i++)
			for (list<Projectile>::iterator mite = missiles.begin(); mite != missiles.end(); mite++)
			{
				//Reduce lifetime of missile
				mite->setLifeTime(mite->getLifeTime() - gEngine->getDt());
				if (mite->getLifeTime() < 0.0f)
				{
					//Remove from scene graph before deleting the object
					mite->removeChildModel(mite->getModel());
					missiles.erase(mite);
					goto stop;
				}
				else
				{
					mite->translate(mite->getDir()*mite->getVel()*gEngine->getDt());

					for (list<GameObject>::iterator ite = objectList.begin(); ite != objectList.end(); ite++)
					{
						if (((*mite).getPos() - ite->getPos()).length() < (*mite).getColRad() + ite->getColRad())
						{
							mite->removeChildModel(mite->getModel());
							(*ite).removeChildModel((*ite).getModel());

							missiles.erase(mite);
							objectList.erase(ite);

							soundManager.play("explosion", glm::vec3(0.0f, 0.0f, 0.0f));
							goto stop;
						}
					}
				}
			}
			for (list<GameObject>::iterator ite = objectList.begin(); ite != objectList.end(); ite++)
			{
				//cout << (player_pos - ite->getPos()).length() << "<" << player.getColRad() + ite->getColRad() << " ? \n";
				if ((player_pos - ite->getPos()).length() < player.getColRad() + ite->getColRad())
				{
					cout << "collision!" << endl;
					ite->removeChildModel(ite->getModel());
					objectList.erase(ite);
					shakeBridge = true;
					shakeTime.setVal(0.0);
					goto stop;
				}
			}
		stop:

			if (shakeBridge) {
				float rand1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float rand2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float rand3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				mNavTrans->postMult(osg::Matrix::rotate(2.5*gEngine->getDt(), rand1, rand2, 0));
				shakeTime.setVal(shakeTime.getVal() + gEngine->getDt());
				if (shakeTime > 0.5){
					shakeBridge = false;
				}

			}
		}
		break;

		case WELCOME_SCREEN: {
			// DO NOTHING FOR NOW. MAYBE SHOW BILLBOARD 
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

	gEngine->setNearAndFarClippingPlanes(0.1f, 500.0f);
	const int * curr_vp = gEngine->getActiveViewportPixelCoords();
	mViewer->getCamera()->setViewport(curr_vp[0], curr_vp[1], curr_vp[2], curr_vp[3]);
	mViewer->getCamera()->setProjectionMatrix(osg::Matrix(glm::value_ptr(gEngine->getActiveViewProjectionMatrix())));

	mViewer->renderingTraversals();
}

void myEncodeFun()
{
	sgct::SharedData::instance()->writeFloat(&curr_time);
	sgct::SharedData::instance()->writeFloat(&navigation_speed);
	sgct::SharedData::instance()->writeFloat(&forward_dir_x);
	sgct::SharedData::instance()->writeFloat(&forward_dir_y);
	sgct::SharedData::instance()->writeFloat(&forward_dir_z);
	sgct::SharedData::instance()->writeFloat(&up_dir_x);
	sgct::SharedData::instance()->writeFloat(&up_dir_y);
	sgct::SharedData::instance()->writeFloat(&up_dir_z);
	sgct::SharedData::instance()->writeFloat(&rotX);
	sgct::SharedData::instance()->writeFloat(&rotY);
	sgct::SharedData::instance()->writeFloat(&rotZ);
	sgct::SharedData::instance()->writeBool(&fireSync);
	sgct::SharedData::instance()->writeBool(&wireframe);
	sgct::SharedData::instance()->writeBool(&info);
	sgct::SharedData::instance()->writeBool(&stats);
	sgct::SharedData::instance()->writeBool(&takeScreenshot);
	sgct::SharedData::instance()->writeBool(&light);
}

void myDecodeFun()
{
	sgct::SharedData::instance()->readFloat(&curr_time);
	sgct::SharedData::instance()->readFloat(&navigation_speed);
	sgct::SharedData::instance()->readFloat(&forward_dir_x);
	sgct::SharedData::instance()->readFloat(&forward_dir_y);
	sgct::SharedData::instance()->readFloat(&forward_dir_z);
	sgct::SharedData::instance()->readFloat(&up_dir_x);
	sgct::SharedData::instance()->readFloat(&up_dir_y);
	sgct::SharedData::instance()->readFloat(&up_dir_z);
	sgct::SharedData::instance()->readFloat(&rotX);
	sgct::SharedData::instance()->readFloat(&rotY);
	sgct::SharedData::instance()->readFloat(&rotZ);
	sgct::SharedData::instance()->readBool(&fireSync);
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
	if (gEngine->isMaster())
	{
		if (state == GAME_SCREEN) {
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
				setGameState(GAME_SCREEN);
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

	// Init game by setting welcome screen
	setGameState(WELCOME_SCREEN);
}

void setupLightSource()
{
	osg::Light * light0 = new osg::Light;
	osg::Light * light1 = new osg::Light;
	osg::LightSource* lightSource0 = new osg::LightSource;
	osg::LightSource* lightSource1 = new osg::LightSource;

	light0->setLightNum(0);
	light0->setPosition(osg::Vec4(5.0f, 5.0f, 10.0f, 1.0f));
	light0->setAmbient(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
	light0->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));
	light0->setSpecular(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	light0->setConstantAttenuation(1.0f);

	lightSource0->setLight(light0);
	lightSource0->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource0->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	light1->setLightNum(1);
	light1->setPosition(osg::Vec4(-5.0f, -2.0f, 10.0f, 1.0f));
	light1->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setDiffuse(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	light1->setSpecular(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	light1->setConstantAttenuation(1.0f);

	lightSource1->setLight(light1);
	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource1->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	mRootNode->addChild(lightSource0);
	mRootNode->addChild(lightSource1);
}


void setGameState(GameState _state) {

	switch (_state) {
		case WELCOME_SCREEN: {
			cout << "State set to WELCOME_SCREEN." << endl;
			state = WELCOME_SCREEN;
		}
		break;
		case GAME_SCREEN: {
			cout << "State set to GAME_SCREEN." << endl;
			//***************************
			//crosshair kod som ska flyttas

			osg::Billboard* crosshairBillBoard = new osg::Billboard();
			mGunnerTrans->addChild(crosshairBillBoard);

			osg::Texture2D *crosshairTexture = new osg::Texture2D;

			crosshairTexture->setImage(osgDB::readImageFile("textures/crosshair.png"));
			cout << "Type of image is: " << crosshairTexture->getImage()->getPixelFormat() << endl;

			osg::StateSet* billBoardStateSet = new osg::StateSet;
			//billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			billBoardStateSet->setTextureAttributeAndModes(0, crosshairTexture, osg::StateAttribute::ON);

			billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
			billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

			// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
			billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

			osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
			billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

			osg::Drawable* crosshairDrawable;
			crosshairDrawable = createCrosshair(1.0, billBoardStateSet);
			crosshairBillBoard->addDrawable(crosshairDrawable, osg::Vec3(0, 5, 0));

			//******************************************


			//Add player object and commandbridge model
			player = GameObject((std::string)("Spelaren"), osg::Vec3f(0, 0, 0), 5.1, (std::string)(""), mPlayerTrans, objIndex++);
			bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 5, -4), 0, (std::string)("models/plattbrygga_3.obj"), mBridgeTrans, objIndex++);


			//Fill scene with 50 asteroids. Later this should be moved to specific scene functions for each level.
			for (int i = 0; i < 50; i++)
			{
				float rand1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float rand2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				float rand3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				objectList.push_back(GameObject((std::string)("en asteroid"), osg::Vec3f(50 - rand1 * 100, 50 - rand2 * 100, 50 - rand3 * 100), 3.0f, (std::string)("models/asteroid.ive"), mSceneTrans, objIndex++));
			}

			soundManager.play("gameOver", glm::vec3(0.0f, 0.0f, 0.0f));
			state = GAME_SCREEN;
		}
		break;
	case GAMEOVER_SCREEN:
		break;
	}

}