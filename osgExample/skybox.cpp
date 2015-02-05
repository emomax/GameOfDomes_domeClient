#include "sgct.h"
#include <iostream>
#include <string>
//#include "enet/enet.h"

#include <thread>
#include <mutex>
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/ComputeBoundsVisitor>
#include <btBulletDynamicsCommon.h>
#include <osg/TextureCubeMap>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osg/TexGen>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "skybox.h"
//#include "Package.h"
sgct::Engine * gEngine;

//Not using ref pointers enables
//more controlled termination
//and prevents segfault on Linux
osgViewer::Viewer * mViewer;
osg::ref_ptr<osg::Group> mRootNode;
osg::ref_ptr<osg::MatrixTransform> mSceneTrans;
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
sgct::SharedDouble curr_time(0.0);
sgct::SharedDouble dist(-2.0);
sgct::SharedBool wireframe(false);
sgct::SharedBool info(false);
sgct::SharedBool stats(false);
sgct::SharedBool takeScreenshot(false);
sgct::SharedBool light(true);

//other var
bool arrowButtons[6];
enum directions { FORWARD = 0, BACKWARD, LEFT, RIGHT, ROLL_RIGHT, ROLL_LEFT };

//navigation vars
float rotationSpeed = 1.0f;
float rollSpeed = 50.0f;
float walkingSpeed = 2.5f;
const double navigation_speed = 5.0;
//to check if left mouse button is pressed
bool mouseLeftButton = false;

/* Holds the difference in position between when the left mouse button
is pressed and when the mouse button is held. */
double mouseDx = 0.0;
double mouseDy = 0.0;

/* Stores the positions that will be compared to measure the difference. */
double mouseXPos[] = { 0.0, 0.0 };
double mouseYPos[] = { 0.0, 0.0 };

glm::vec3 view(0.0f, 0.0f, 1.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 pos(0.0f, 0.0f, 0.0f);

sgct::SharedObject<glm::mat4> xform;


// Bullet
btBroadphaseInterface* broadphase;
btDefaultCollisionConfiguration* collisionConfiguration;
btCollisionDispatcher* dispatcher;
btSequentialImpulseConstraintSolver* solver;
btDiscreteDynamicsWorld* dynamicsWorld;
btCollisionShape* groundShape;
btCollisionShape* fallShape;
btDefaultMotionState* groundMotionState;
btRigidBody* groundRigidBody;
btDefaultMotionState* fallMotionState;
btRigidBody* fallRigidBody;

int main(int argc, char* argv[])
{
	// Bullet
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, -1, 0));
	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	fallShape = new btSphereShape(0.001);
	groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 2, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	dynamicsWorld->addRigidBody(fallRigidBody);

	// SGCT

	gEngine = new sgct::Engine(argc, argv);
	gEngine->setInitOGLFunction(myInitOGLFun);
	gEngine->setPreSyncFunction(myPreSyncFun);
	gEngine->setPostSyncPreDrawFunction(myPostSyncPreDrawFun);
	gEngine->setDrawFunction(myDrawFun);
	gEngine->setCleanUpFunction(myCleanUpFun);
	gEngine->setKeyboardCallbackFunction(keyCallback);

	for (int i = 0; i<4; i++)
		arrowButtons[i] = false;

	if (!gEngine->init())
	{
		delete gEngine;
		return EXIT_FAILURE;
	}

	sgct::SharedData::instance()->setEncodeFunction(myEncodeFun);
	sgct::SharedData::instance()->setDecodeFunction(myDecodeFun);

	// Main loop
	gEngine->render();

	delete gEngine;
	dynamicsWorld->removeRigidBody(fallRigidBody);
	delete fallRigidBody->getMotionState();
	delete fallRigidBody;

	dynamicsWorld->removeRigidBody(groundRigidBody);
	delete groundRigidBody->getMotionState();
	delete groundRigidBody;


	delete fallShape;

	delete groundShape;


	delete dynamicsWorld;
	delete solver;
	delete collisionConfiguration;
	delete dispatcher;
	delete broadphase;

	// Exit program
	exit(EXIT_SUCCESS);
}

void myInitOGLFun()
{
	initOSG();

	sgct::MessageHandler::instance()->print("Loading model 'spaceshipFul.obj'...\n");
	osg::ref_ptr<osg::Node> mModel = osgDB::readNodeFile("spaceshipFul.3ds");
	osg::ref_ptr<osg::MatrixTransform> mModelTrans;

	gEngine->setNearAndFarClippingPlanes(0.1f, 500.0f);

	// SkyBox from OSG Cookbook
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 5.0f)));

	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	/*skybox->setEnvironmentMap( 0,
	osgDB::readImageFile("BlueChecker.png"), osgDB::readImageFile("OrangeChecker.png"),
	osgDB::readImageFile("GreenChecker.png"), osgDB::readImageFile("YellowChecker.png"),
	osgDB::readImageFile("RedChecker.png"), osgDB::readImageFile("PurpleChecker.png") );*/
	skybox->setEnvironmentMap(0,
		osgDB::readImageFile("stars.png"), osgDB::readImageFile("stars.png"),
		osgDB::readImageFile("stars.png"), osgDB::readImageFile("stars.png"),
		osgDB::readImageFile("stars.png"), osgDB::readImageFile("stars.png"));
	skybox->addChild(geode.get());
	mSceneTrans = new osg::MatrixTransform();
	mModelTrans = new osg::MatrixTransform();

	//rotate osg coordinate system to match sgct
	mModelTrans->preMult(osg::Matrix::rotate(glm::radians(-90.0f),
		1.0f, 0.0f, 0.0f));

	mRootNode->addChild(skybox.get());
	mRootNode->addChild(mSceneTrans.get());
	mSceneTrans->addChild(mModelTrans.get());

	if (mModel.valid())
	{
		sgct::MessageHandler::instance()->print("Model loaded successfully!\n");
		mModelTrans->addChild(mModel.get());

		//get the bounding box
		osg::ComputeBoundsVisitor cbv;
		osg::BoundingBox &bb(cbv.getBoundingBox());
		mModel->accept(cbv);

		osg::Vec3f tmpVec;
		tmpVec = bb.center();


		//scale to fit model and translate model center to origin
		mModelTrans->postMult(osg::Matrix::translate(-tmpVec));
		mModelTrans->postMult(osg::Matrix::scale(1.0f / bb.radius(), 1.0f / bb.radius(), 1.0f / bb.radius()));

		sgct::MessageHandler::instance()->print("Model bounding sphere center:\tx=%f\ty=%f\tz=%f\n", tmpVec[0], tmpVec[1], tmpVec[2]);
		sgct::MessageHandler::instance()->print("Model bounding sphere radius:\t%f\n", bb.radius());

		//disable face culling
		mModel->getOrCreateStateSet()->setMode(GL_CULL_FACE,
			osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

		geode->getOrCreateStateSet()->setMode(GL_CULL_FACE,
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
		int width, height;
		width = gEngine->getActiveXResolution();
		height = gEngine->getActiveYResolution();

		sgct::Engine::getMousePos(gEngine->getFocusedWindowIndex(), &mouseXPos[0], &mouseYPos[0]);
		mouseDx = mouseXPos[0] - width / 2;
		mouseDy = mouseYPos[0] - height / 2;

		sgct::Engine::setMousePos(gEngine->getFocusedWindowIndex(), width / 2, height / 2);

		static float panRot = 0.0f;
		panRot += (static_cast<float>(mouseDx)* rotationSpeed * static_cast<float>(gEngine->getDt()));
		static float vertRot = 0.0f;
		vertRot += (static_cast<float>(mouseDy)* rotationSpeed * static_cast<float>(gEngine->getDt()));

		static float rollRot = 0.0f;
		if (arrowButtons[ROLL_RIGHT]){
			sgct::MessageHandler::instance()->print("Rolling Right...\n");
			rollRot += (rollSpeed * static_cast<float>(gEngine->getDt()));
		}
		if (arrowButtons[ROLL_LEFT]){
			sgct::MessageHandler::instance()->print("Rolling Left...\n");
			rollRot -= (rollSpeed * static_cast<float>(gEngine->getDt()));
		}

		// std::cout << rollRot << std::endl;

		glm::mat4 ViewRotateX = glm::rotate(
			glm::mat4(1.0f),
			panRot,
			glm::vec3(0.0f, 1.0f, 0.0f)); //rotation around the y-axis

		glm::mat4 ViewRotateY = glm::rotate(
			glm::mat4(1.0f),
			vertRot,
			glm::vec3(1.0f, 0.0f, 0.0f)); //rotation around the x-axis

		glm::mat4 ViewRotateZ = glm::rotate(
			glm::mat4(1.0f),
			rollRot,
			glm::vec3(0.0f, 0.0f, 1.0f));


		glm::mat4 ViewMat = ViewRotateZ * ViewRotateY * ViewRotateX;

		view = glm::inverse(glm::mat3(ViewMat)) * glm::vec3(0.0f, 0.0f, -1.0f);

		glm::vec4 up4 = ViewRotateZ * ViewRotateY *  glm::vec4(up, 1.0f);
		glm::vec3 newup = glm::vec3(up4.x / up4.w, up4.y / up4.w, up4.z / up4.w);

		// glm::vec4 pos4 = ViewRotateZ * ViewRotateX * glm::vec4(pos, 1.0f);
		// pos = glm::vec3(pos4.x/pos4.w, pos4.y/pos4.w, pos4.z/pos4.w);

		glm::vec3 right = glm::cross(view, newup);

		newup = glm::cross(right, view);

		std::cout << "X: " << newup[0] << "  Y: " << newup[1] << "  Z: " << newup[2] << std::endl;

		right = glm::cross(view, newup);


		if (arrowButtons[FORWARD]){

			pos += (walkingSpeed * static_cast<float>(gEngine->getDt()) * view);
			view += (walkingSpeed * static_cast<float>(gEngine->getDt()) * view);

		}

		if (arrowButtons[BACKWARD]){

			pos -= (walkingSpeed * static_cast<float>(gEngine->getDt()) * view);
			view -= (walkingSpeed * static_cast<float>(gEngine->getDt()) * view);

		}

		if (arrowButtons[LEFT]){

			pos -= (walkingSpeed * static_cast<float>(gEngine->getDt()) * right);
			view -= (walkingSpeed * static_cast<float>(gEngine->getDt()) * right);

		}

		if (arrowButtons[RIGHT]){

			pos += (walkingSpeed * static_cast<float>(gEngine->getDt()) * right);
			view += (walkingSpeed * static_cast<float>(gEngine->getDt()) * right);

		}


		osg::Vec3d vPos(pos.x, pos.y, pos.z);
		osg::Vec3d vView(view.x, view.y, view.z);
		osg::Vec3d vUp(up.x, up.y, up.z);

		mViewer->getCamera()->setViewMatrixAsLookAt(vPos, vView, vUp);

		// glm::mat4 result;
		// result = glm::translate( glm::mat4(1.0f), sgct::Engine::getUserPtr()->getPos() );
		// //2. apply transformation
		// result *= (ViewMat *  glm::translate( glm::mat4(1.0f), pos ));
		// //1. transform user to coordinate system origin
		// result *= glm::translate( glm::mat4(1.0f), -sgct::Engine::getUserPtr()->getPos() );

		// xform.setVal( result );
	}
}

void myPostSyncPreDrawFun()
{

	dynamicsWorld->stepSimulation(1 / 60.f, 10);

	btTransform trans;
	fallRigidBody->getMotionState()->getWorldTransform(trans);

	//std::cout << "sphere height: " << trans.getOrigin().getY() << std::endl;


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

	mSceneTrans->setMatrix(osg::Matrix::rotate(glm::radians(curr_time.getVal() * 8.0), 0.0, 1.0, 0.0));
	mSceneTrans->postMult(osg::Matrix::translate(0.0, trans.getOrigin().getY(), dist.getVal()));

	//transform to scene transformation from configuration file
	mSceneTrans->postMult(osg::Matrix(glm::value_ptr(gEngine->getModelMatrix())));

	//update the frame stamp in the viewer to sync all
	//time based events in osg
	mFrameStamp->setFrameNumber(gEngine->getCurrentFrameNumber());
	mFrameStamp->setReferenceTime(curr_time.getVal());
	mFrameStamp->setSimulationTime(curr_time.getVal());
	mViewer->setFrameStamp(mFrameStamp.get());
	mViewer->advance(curr_time.getVal()); //update

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
	const int * curr_vp = gEngine->getActiveViewportPixelCoords();
	mViewer->getCamera()->setViewport(curr_vp[0], curr_vp[1], curr_vp[2], curr_vp[3]);
	mViewer->getCamera()->setProjectionMatrix(osg::Matrix(glm::value_ptr(gEngine->getActiveViewProjectionMatrix())));
	mViewer->renderingTraversals();
}

void myEncodeFun()
{
	sgct::SharedData::instance()->writeDouble(&curr_time);
	sgct::SharedData::instance()->writeDouble(&dist);
	sgct::SharedData::instance()->writeBool(&wireframe);
	sgct::SharedData::instance()->writeBool(&info);
	sgct::SharedData::instance()->writeBool(&stats);
	sgct::SharedData::instance()->writeBool(&takeScreenshot);
	sgct::SharedData::instance()->writeBool(&light);
}

void myDecodeFun()
{
	sgct::SharedData::instance()->readDouble(&curr_time);
	sgct::SharedData::instance()->readDouble(&dist);
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
	if (gEngine->isMaster()) {
		switch (key){

		case SGCT_KEY_UP:
		case SGCT_KEY_W:
			arrowButtons[FORWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case SGCT_KEY_DOWN:
		case SGCT_KEY_S:
			arrowButtons[BACKWARD] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case SGCT_KEY_LEFT:
		case SGCT_KEY_A:
			arrowButtons[LEFT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case SGCT_KEY_RIGHT:
		case SGCT_KEY_D:
			arrowButtons[RIGHT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case SGCT_KEY_Q:
			arrowButtons[ROLL_LEFT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
			break;

		case SGCT_KEY_E:
			arrowButtons[ROLL_RIGHT] = ((action == SGCT_REPEAT || action == SGCT_PRESS) ? true : false);
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
	mViewer->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
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
	light0->setAmbient(osg::Vec4(0.3f, 0.3f, 0.3f, 1.0f));
	light0->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light0->setSpecular(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	light0->setConstantAttenuation(1.0f);

	lightSource0->setLight(light0);
	lightSource0->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource0->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	light1->setLightNum(1);
	light1->setPosition(osg::Vec4(-5.0f, -2.0f, 10.0f, 1.0f));
	light1->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	light1->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	light1->setSpecular(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	light1->setConstantAttenuation(1.0f);

	lightSource1->setLight(light1);
	lightSource1->setLocalStateSetModes(osg::StateAttribute::ON);
	lightSource1->setStateSetModes(*(mRootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	mRootNode->addChild(lightSource0);
	mRootNode->addChild(lightSource1);
}