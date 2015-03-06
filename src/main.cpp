#include "sgct.h"

#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<vector>

#include "classroom\SkyBox.h"
#include "classroom\Projectile.h"

sgct::Engine * gEngine;

//Not using ref pointers enables
//more controlled termination
//and prevents segfault on Linux
osgViewer::Viewer * mViewer;
osg::ref_ptr<osg::Group> mRootNode;
osg::ref_ptr<osg::MatrixTransform> mNavTrans;
osg::ref_ptr<osg::MatrixTransform> mSceneTrans;
osg::ref_ptr<osg::MatrixTransform> mPlayerTrans;



osg::Vec3d forward_dir;
osg::Vec3d player_pos;
osg::ref_ptr<osg::FrameStamp> mFrameStamp; //to sync osg animations across cluster

//From cookbook for camera to track model
//osg::ref_ptr<osgGA::NodeTrackerManipulator> nodeTracker = new osgGA::NodeTrackerManipulator;

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
sgct::SharedDouble curr_time(0.0);
sgct::SharedDouble forward_speed(0.0);
sgct::SharedDouble rotation_x(0.0);
sgct::SharedDouble rotation_y(1.57);
sgct::SharedBool wireframe(false);
sgct::SharedBool info(false);
sgct::SharedBool stats(false);
sgct::SharedBool takeScreenshot(false);
sgct::SharedBool light(true);

//other var
bool Buttons[7];
enum directions { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, SHOOT };
double navigation_speed = 0.0;
const double turn_speed = 3.0;
std::vector<Projectile> missiles;


int main(int argc, char* argv[])
{
	gEngine = new sgct::Engine(argc, argv);

	gEngine->setInitOGLFunction(myInitOGLFun);
	gEngine->setPreSyncFunction(myPreSyncFun);
	gEngine->setPostSyncPreDrawFunction(myPostSyncPreDrawFun);
	gEngine->setDrawFunction(myDrawFun);
	gEngine->setCleanUpFunction(myCleanUpFun);
	gEngine->setKeyboardCallbackFunction(keyCallback);

	//From cookbook
	//nodeTracker->setHomePosition(osg::Vec3(0.0, 0.5, 0.0), osg::Vec3(0.0, 0.0, 0.0), osg::Z_AXIS);
	//nodeTracker->setTrackerMode(osgGA::NodeTrackerManipulator::NODE_CENTER_AND_ROTATION);
	//nodeTracker->setRotationMode(osgGA::NodeTrackerManipulator::TRACKBALL);

	//fix incompability with warping and OSG
	sgct_core::ClusterManager::instance()->setMeshImplementation(sgct_core::ClusterManager::DISPLAY_LIST);

	for (int i = 0; i<7; i++)
		Buttons[i] = false;

	if (!gEngine->init())
	{
		delete gEngine;
		return EXIT_FAILURE;
	}

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
	initOSG();

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 90)));  //scene->getBound().radius())));
	geode->setCullingActive(false);

	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	skybox->setEnvironmentMap(0,
		osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_right1.png"), osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_left2.png"),
		osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_bottom4.png"), osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_top3.png"),
		osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_front5.png"), osgDB::readImageFile("textures/tobpi_maxjo_skyboxtest1_back6.png"));
	skybox->addChild(geode.get());
	
	osg::ref_ptr<osg::Group> root = new osg::Group;

	osg::ref_ptr<osg::Node> mModel;
	
	mNavTrans = new osg::MatrixTransform();
	mSceneTrans = new osg::MatrixTransform();
	mPlayerTrans = new osg::MatrixTransform();

	mPlayerTrans->setMatrix(osg::Matrix::identity());

	//add skybox to the scene graph
	mRootNode->addChild(mPlayerTrans.get());
	mRootNode->addChild(mNavTrans.get());
	
	mNavTrans->addChild(mSceneTrans.get());
	mSceneTrans->addChild(skybox.get());
	
	srand(time(NULL));
	for (int i = 0; i < 10; i++)
	{
		float rand1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		float rand3 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		std::cout << "asteroid nummer: " << i << "\n";
		GameObject hej = GameObject((std::string)("ettplan"), osg::Vec3f(20 - rand1 * 200, rand2 * 10, rand3 * 80), (std::string)("models/airplane.ive"), mSceneTrans);
	}

	sgct::MessageHandler::instance()->print("Loading model 'airplane.ive'...\n");
	mModel = osgDB::readNodeFile("models/airplane.ive");

	if (mModel.valid())
	{
		sgct::MessageHandler::instance()->print("Model loaded successfully!\n");
		mPlayerTrans->addChild(mModel.get());

		//get the bounding box
		osg::ComputeBoundsVisitor cbv;
		osg::BoundingBox &bb(cbv.getBoundingBox());
		mModel->accept(cbv);

		osg::Vec3f tmpVec;
		tmpVec = bb.center();

		//scale to fit model and translate model center to origin
		mPlayerTrans->postMult(osg::Matrix::rotate(-PI/2.0, 1.0, 0.0, 0.0));
		mPlayerTrans->postMult(osg::Matrix::rotate(PI, 0.0, 1.0, 0.0));
		mPlayerTrans->postMult(osg::Matrix::translate(0.0, -0.5, 0.0));
		mPlayerTrans->preMult(osg::Matrix::scale(1.0f / bb.radius(), 1.0f / bb.radius(), 1.0f / bb.radius()));

		sgct::MessageHandler::instance()->print("Model bounding sphere center:\tx=%f\ty=%f\tz=%f\n", tmpVec[0], tmpVec[1], tmpVec[2]);
		sgct::MessageHandler::instance()->print("Model bounding sphere radius:\t%f\n", bb.radius());

		//disable face culling
		mModel->getOrCreateStateSet()->setMode(GL_CULL_FACE,
			osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
	else
		sgct::MessageHandler::instance()->print("Failed to read model!\n");

	setupLightSource();
}

void myPreSyncFun()
{
	if (gEngine->isMaster())
	{
		curr_time.setVal(sgct::Engine::getTime());

		//Handle key-input (not the real key input but what happens when a specific key has been pressed)
		if (Buttons[FORWARD] && navigation_speed < 0.8)
			navigation_speed += 0.01;
		if (Buttons[BACKWARD] && navigation_speed > -0.5)
			navigation_speed = -0.01;
		if (Buttons[RIGHT])
			if (rotation_x.getVal() > 0.0)
				rotation_x.setVal(rotation_x.getVal() - (turn_speed * gEngine->getDt()));
			else
				rotation_x.setVal(2*PI);
		if (Buttons[LEFT])
			if (rotation_x.getVal() < 2*PI)
				rotation_x.setVal(rotation_x.getVal() + (turn_speed * gEngine->getDt()));
			else
				rotation_x.setVal(0.0);
		if (Buttons[UP] && rotation_y.getVal() < PI)
			rotation_y.setVal(rotation_y.getVal() + (turn_speed * gEngine->getDt()));
		if (Buttons[DOWN] && rotation_y.getVal() > 0.0)
			rotation_y.setVal(rotation_y.getVal() - (turn_speed * gEngine->getDt()));
		if (Buttons[SHOOT])
		{
			missiles.push_back(Projectile((std::string)("ettskott"), osg::Vec3f(0.0, 0.0, 10.0), (std::string)("models/airplane.ive"), mSceneTrans, 1.0f, -0.3f));
			Buttons[SHOOT] = false;
			
			for (int i = 1; i < missiles.size(); i++)
			{
				Projectile temp = missiles[missiles.size() - i];
				missiles[missiles.size() - i] = missiles[missiles.size()-i-1];
				missiles[missiles.size()-i-1] = temp;
			}
		}
	}
}

void myPostSyncPreDrawFun()
{
	gEngine->setWireframe(wireframe.getVal());
	gEngine->setDisplayInfoVisibility(info.getVal());
	gEngine->setStatsGraphVisibility(stats.getVal());

	if (takeScreenshot.getVal())
	{
		gEngine->takeScreenshot();
		takeScreenshot.setVal(false);
	}

	light.getVal() ? mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE) :
		mRootNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	mNavTrans->setMatrix(osg::Matrix::identity());
	
	//rotate osg coordinate system to match sgct
	mNavTrans->preMult(osg::Matrix::rotate(-PI/2, 1.0f, 0.0f, 0.0f));

	forward_dir = osg::Vec3d(-sin(rotation_x.getVal())*sin(rotation_y.getVal()), -cos(rotation_y.getVal()), -cos(rotation_x.getVal())*sin(rotation_y.getVal()));
	player_pos = player_pos + forward_dir*navigation_speed;

	//mNavTrans->postMult(osg::Matrix::scale(1.0f / 10.0f, 1.0f / 10.0f, 1.0f / 10.0f));
	mNavTrans->postMult(osg::Matrix::rotate(rotation_x.getVal(), 0.0, 1.0, 0.0));
	mNavTrans->postMult(osg::Matrix::rotate(rotation_y.getVal(), cos(rotation_x.getVal()), 0.0, -sin(rotation_x.getVal()) ));
	mNavTrans->postMult(osg::Matrix::translate(player_pos));
	mNavTrans->setMatrix(mNavTrans->getInverseMatrix());

	//transform to scene transformation from configuration file
	mSceneTrans->setMatrix(osg::Matrix(glm::value_ptr(gEngine->getModelMatrix())));

	if (missiles.size() > 10)
	{
		missiles[missiles.size() - 1].removeChildModel(missiles[missiles.size()-1].getModel());
		missiles.pop_back();
	}
	for (int i = 0; i < missiles.size(); i++)
	{
		missiles[i].translate(missiles[i].getDir()*missiles[i].getVel());
	}
	//missiles.clear();

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

	mViewer->renderingTraversals();
}

void myEncodeFun()
{
	sgct::SharedData::instance()->writeDouble(&curr_time);
	sgct::SharedData::instance()->writeDouble(&forward_speed);
	sgct::SharedData::instance()->writeDouble(&rotation_x);
	sgct::SharedData::instance()->writeDouble(&rotation_y);
	sgct::SharedData::instance()->writeBool(&wireframe);
	sgct::SharedData::instance()->writeBool(&info);
	sgct::SharedData::instance()->writeBool(&stats);
	sgct::SharedData::instance()->writeBool(&takeScreenshot);
	sgct::SharedData::instance()->writeBool(&light);
}

void myDecodeFun()
{
	sgct::SharedData::instance()->readDouble(&curr_time);
	sgct::SharedData::instance()->readDouble(&forward_speed);
	sgct::SharedData::instance()->readDouble(&rotation_x);
	sgct::SharedData::instance()->readDouble(&rotation_y);
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

void keyCallback(int key, int action)
{
	if (gEngine->isMaster())
	{
		switch (key)
		{
		case 'Z':
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
			Buttons[FORWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case 'E':
			Buttons[BACKWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
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
