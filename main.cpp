/*
osgExample_sfs
*/

#include "sgct.h"

#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<vector>

#include "classroom\SkyBox.h"
#include "classroom\Projectile.h"
#include "classroom\EnemyShip.h"
#include "classroom\NetworkManager.h"

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


//Position and direction variables for the player
osg::Vec3f player_pos = osg::Vec3f(0,0,0);
//(0, forward_dir)
osg::Quat baseQuat = osg::Quat(0, 0, 1, 0);

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

//Temporary variables that will be replaced after merge with SFS
bool Buttons[9];
enum directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, ROLLLEFT, ROLLRIGHT, SHOOT };

const float accRotVal = 0.005;
const float accRotMax = 0.4;
const float accThrustVal = 0.006;
const float accThrustMax = 0.4;
float accRotX = 0.0;
float accRotY = 0.0;
float accRotZ = 0.0;
float accThrust = 0.0;

//Convert player direction to osg format
osg::Vec3f osg_forward_dir = osg::Vec3f(forward_dir_x.getVal(), forward_dir_y.getVal(), forward_dir_z.getVal());
osg::Vec3f osg_up_dir = osg::Vec3f(up_dir_x.getVal(), up_dir_y.getVal(), up_dir_z.getVal());

//vector containing all projectiles in the scene
std::vector<Projectile> missiles;

//vector containg all objects (asteroids) in the scene
std::vector<GameObject> objectList;

const float fireRate = 0.4; //One bullet / 400ms
const float projectileVelocity = 40.0;
float fireTimer = 0.0;


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

		bool pInputUp = *(ptrNotifiedISFSObject->GetBool("sgctUp"));
		bool pInputDown = *(ptrNotifiedISFSObject->GetBool("sgctDown"));
		bool pInputLeft = *(ptrNotifiedISFSObject->GetBool("sgctLeft"));
		bool pInputRight = *(ptrNotifiedISFSObject->GetBool("sgctRight"));
		bool pInputForward = *(ptrNotifiedISFSObject->GetBool("sgctForward"));
		bool pInputBackward = *(ptrNotifiedISFSObject->GetBool("sgctBackward"));

		// Update velocities based on mobile input 
		if (pInputDown) {
			if (accRotX < accRotMax)
				accRotX += accRotVal;
		}
		else if (pInputUp) {
			if (accRotX > -accRotMax)
				accRotX -= accRotVal;
		}

		if (pInputLeft) {
			if (accRotZ > -accRotMax)
				accRotZ -= accRotVal;
		}
		else if (pInputRight) {
			if (accRotZ < accRotMax)
				accRotZ += accRotVal;
		}

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

		bool up = *(ptrNotifiedISFSObject->GetBool("sgctUp"));
		bool down = *(ptrNotifiedISFSObject->GetBool("sgctDown"));
		bool left = *(ptrNotifiedISFSObject->GetBool("sgctLeft"));
		bool right = *(ptrNotifiedISFSObject->GetBool("sgctRight"));
		bool fire = *(ptrNotifiedISFSObject->GetBool("sgctFire"));

		if (fire && fireTimer <= 0.0 ) {
			fireSync.setVal(true);
			fireTimer = fireRate;
		}
	}
}


// Handle networking
NetworkManager manager;



int main(int argc, char* argv[])
{
	manager.init();

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
	mPlayerTrans->setMatrix(osg::Matrix::identity());
	

	//Setup the scene graph
	mRootNode->addChild(mPlayerTrans);
	mRootNode->addChild(mNavTrans);
	mNavTrans->addChild(mSceneTrans);
	mNavTrans->addChild(skybox);

	//Add player model
	GameObject player = GameObject((std::string)("ettplan"), osg::Vec3f(0.0, 0.0, 0.0), (std::string)(""), mPlayerTrans);

	//Låt stå för referens
	mPlayerTrans->postMult(osg::Matrix::rotate(-PI / 2.0 - 35 * PI / 180, 1.0, 0.0, 0.0));

	//Fill scene with 50 asteroids. Later this should be moved to specific scene functions for each level.
	for (int i = 0; i < 50; i++)
	{
		float rand1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		std::cout << "Succesfully loaded asteroid: " << i << "\n";
		objectList.push_back(GameObject((std::string)("en asteroid"), osg::Vec3f(50 - rand1 * 100, 50 - rand2 * 100, 50 - rand3 * 100), (std::string)("models/asteroid.ive"), mSceneTrans));
	}

	//Setup the lightsource
	setupLightSource();
}

void myPreSyncFun()
{
	if (gEngine->isMaster())
	{
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
			fireSync.setVal(true);
			fireTimer = fireRate;
		}
	}
}

void myPostSyncPreDrawFun()
{
	if (fireSync.getVal())
	{
		//Add and then sort new projectiles in the missile vector.
		missiles.push_back(Projectile((std::string)("ettskott"), player_pos + baseQuat * osg::Vec3f(2, 0, 1), osg_forward_dir, baseQuat, (std::string)("models/skott.ive"), mSceneTrans, 1.0f, projectileVelocity));
		for (int i = 1; i < missiles.size(); i++)
		{
			Projectile temp = missiles[missiles.size() - i];
			missiles[missiles.size() - i] = missiles[missiles.size() - i - 1];
			missiles[missiles.size() - i - 1] = temp;
		}
		missiles.push_back(Projectile((std::string)("ettskott"), player_pos + baseQuat * osg::Vec3f(-2, 0, 1), osg_forward_dir, baseQuat, (std::string)("models/skott.ive"), mSceneTrans, 1.0f, projectileVelocity));
		for (int i = 1; i < missiles.size(); i++)
		{
			Projectile temp = missiles[missiles.size() - i];
			missiles[missiles.size() - i] = missiles[missiles.size() - i - 1];
			missiles[missiles.size() - i - 1] = temp;
		}
		fireSync.setVal(false);
	}

	//Check OsgExample settings
	gEngine->setWireframe(wireframe.getVal());
	gEngine->setDisplayInfoVisibility(info.getVal());
	gEngine->setStatsGraphVisibility(stats.getVal());

	//Probably not very useful
	if (takeScreenshot.getVal())
	{
		gEngine->takeScreenshot();
		takeScreenshot.setVal(false);
	}

	//Enable or disable light with the L button
	light.getVal() ? mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE) :
		mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	//theta och phi problemet
	//if (phi > PI)
	//	theta.setVal(-theta.getVal());

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
	for (int i = 0; i < missiles.size(); i++)
	{
		//Reduce lifetime of missile
		missiles[i].setLifeTime(missiles[i].getLifeTime() - gEngine->getDt());
		if (missiles[i].getLifeTime() < 0.0f)
		{
			//Remove from scene graph before deleting the object
			missiles[i].removeChildModel(missiles[i].getModel());

			//Place projectile last in vector so we can use pop_back() correctly
			Projectile temp = missiles[missiles.size() - 1];
			missiles[missiles.size() - 1] = missiles[i];
			missiles[i] = temp;

			missiles.pop_back();
		}
		else
		{
			missiles[i].translate(missiles[i].getDir()*missiles[i].getVel()*gEngine->getDt());

			//Remove asteroid and missile if collision occurs
			for (int j = 0; j < objectList.size(); j++)
			{
				if ((missiles[i].getPos() - objectList[j].getPos()).length() < missiles[i].getColRad() + objectList[j].getColRad())
				{
					cout << (missiles[i].getPos() - objectList[j].getPos()).length() << " < " << missiles[i].getColRad() + objectList[j].getColRad() << " ? \n";
					missiles[i].removeChildModel(missiles[i].getModel());
					objectList[j].removeChildModel(objectList[j].getModel());
					//objectList[j].setModel("models/airplane.ive");
					//Place projectile and asteroid last in vector so we can use pop_back() correctly
					Projectile tempProj = missiles[missiles.size() - 1];
					missiles[missiles.size() - 1] = missiles[i];
					missiles[i] = tempProj;
					GameObject tempObj = objectList[objectList.size() - 1];
					objectList[objectList.size() - 1] = objectList[j];
					objectList[j] = tempObj;
					
					missiles.pop_back();
					objectList.pop_back();

					j = objectList.size();
					i = missiles.size();
				}
			}
		}
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

	const int * curr_vp = gEngine->getActiveViewportPixelCoords();
	mViewer->getCamera()->setViewport(curr_vp[0], curr_vp[1], curr_vp[2], curr_vp[3]);
	mViewer->getCamera()->setProjectionMatrix(osg::Matrix(glm::value_ptr(gEngine->getActiveViewProjectionMatrix())));
	//mViewer->getCamera()->setViewMatrix(osg::Matrix::lookAt(osg_forward_dir, player_pos, osg::Vec3(0, 1, 0)));

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
