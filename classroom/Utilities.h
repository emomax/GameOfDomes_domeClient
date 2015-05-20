/*
Utilities.h contains useful functions that the program uses.
*/

#include "Includes.h"

#include "SkyBox.h"
#include "Projectile.h"
#include "Player.h"
#include "Billboard.h"
#include "NetworkManager.h"
#include "SoundManager.h"

// CREATE ANIMATION SEQUENCE
osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;

//Function declarations
void createBillboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height);
void makeSkyBox(osg::ref_ptr<osg::MatrixTransform> _mNavTrans);
void createExplosion(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height);


//Function for changing level. the list containing all objects and the relevant matrix transforms are called as reference. Note that the matrix transforms are pointers.
void setGameState(int _state, int& _objIndex, std::list<GameObject*>& _objList, std::list<Billboard>& _billList, Player& _player, osg::ref_ptr<osg::MatrixTransform> _mNavTrans,
	osg::ref_ptr<osg::MatrixTransform> _mRootTrans, osg::ref_ptr<osg::MatrixTransform> _mSceneTrans, osg::ref_ptr<osg::MatrixTransform> _mWelcomeTrans,
	SoundManager& _soundManager, int _randomSeed, int _asteroidAmount, bool _isMaster)
{

	switch (_state) {
		//Welcome Screen
	case 0: {
				cout << "State set to WELCOME_SCREEN." << endl;

				_mSceneTrans->removeChildren(0, _mSceneTrans->getNumChildren());
				_objList.clear();
				_billList.clear();
				_objIndex = 0;

				//Check if game just started or if game over reset. RootTrans should only have navTrans and welcomeTrans as children at start of game
				if (_mRootTrans->getNumChildren() <= 2)
					makeSkyBox(_mNavTrans);
				else
					_player.resetPlayer();

				_billList.push_back(Billboard(30.0, osg::Vec3f(0, 30, 0), "textures/dome_startscreen.png", _mWelcomeTrans, 1.0, 1.0, "Startscreen"));
				//createBillboard(1.0, osg::Vec3f(0, 3, 0), "textures/dome_startscreen.png", _mWelcomeTrans, 3.0, 3.0);
				if (_isMaster)
					_soundManager.play("mainMenu_music", osg::Vec3f(0.0f, 0.0f, 0.0f));
				_state = 0;

				imageSequence->setLength(1.2);
				// load images
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00001.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00002.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00003.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00004.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00005.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00006.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00007.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00008.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00009.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00010.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00011.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00012.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00013.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00014.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00015.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00016.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00017.png"));
				imageSequence->addImage(osgDB::readImageFile("textures/Explosion_01_00018.png"));

				//imageSequence->setLoopingMode(osg::ImageStream::NO_LOOPING);
				imageSequence->play();
	}
		break;

		//Game Screen
	case 1: {
				cout << "State set to GAME_SCREEN." << endl;

				_mSceneTrans->removeChildren(0, _mSceneTrans->getNumChildren());
				_mWelcomeTrans->removeChildren(0, _mWelcomeTrans->getNumChildren());
				_billList.clear();

				//Create the player. This will create matrix-transforms for the commandbridge and gunner as well.
				_player = Player("Player1", osg::Vec3f(0, 0, 0), 250, 500, _mRootTrans);

				//Create the crosshair and set to be child of mGunnerTrans
				_billList.push_back(Billboard(0.5, osg::Vec3f(0, 2.5, 0), "textures/crosshair.png", _player.getGunnerTrans(), 1.0, 1.0, "Crosshair"));
				//createBillboard(0.5, osg::Vec3f(0, 2.5, 0), "textures/crosshair.png", _player.getGunnerTrans(), 1.0, 1.0);
				

				//Create a billboard representing the sun
				_billList.push_back(Billboard(80000.0, osg::Vec3f(80000, 0, 0), "textures/sol.png", _mSceneTrans, 1.0, 1.0, "Sun"));
				//createBillboard(80000.0, osg::Vec3f(80000, 0, 0), "textures/sol.png", _mSceneTrans, 1.0, 1.0);

				//Fill scene with 50 asteroids.
				for (int i = 0; i < _asteroidAmount; i++)
				{
					int rand1 = 50000 - (_randomSeed * 3571 + 997) % 100000;  //generate random value between -5000 and 5000
					int rand2 = 50000 - ((50000 + rand1) * 3571 + 997) % 100000; //generate new random value between -5000 and 5000
					int rand3 = 50000 - ((50000 + rand2) * 3571 + 997) % 100000; //Prime numbers are used to avoid repetitions.

					//float randScale = 1.2 - (float)(((500 + rand3) * 3571 + 997) % 400) / 1000; //generate random value between 0.8 - 1.2
					//randScale *= 10;

					_randomSeed = 50000 + rand3;

					_objList.push_back(new GameObject((std::string)("Asteroid"), osg::Vec3f(rand1, rand2, rand3), 2500.0f, 500, (std::string)("models/asteroid_5meter.ive"), _mSceneTrans, _objIndex++));
					//_objList.back()->setScale(randScale);						//pos variable need to take the scale into account. Save for later.
					_objList.back()->rotate(osg::Quat(_randomSeed, rand1, rand2, rand3));
					std::cout << _objList.back()->getName() << _objList.back()->getID() << std::endl;
				}
				if (_isMaster)
					_soundManager.play("inGame_music", osg::Vec3f(0.0f, 0.0f, 0.0f));

				_state = 1;
	}
		break;

		//Gameover Screen
	case 2: {
				_state = 2;
	}
		break;
	case 3: {
				cout << "State set to PREGAME_SCREEN." << endl;
				if (_isMaster)
					_soundManager.play("preGame_music", osg::Vec3f(0.0f, 0.0f, 0.0f));
				_state = 3;
	}
		break;
	}

}


//Billboard functions
/*
#pragma once
osg::Drawable* createBillboardDrawable(const float & scale, osg::StateSet* bbState, float width, float height)
{
	width *= scale;
	height *= scale;

	osg::Geometry* billboardQuad = new osg::Geometry;

	osg::Vec3Array* crosshairVerts = new osg::Vec3Array(4);
	(*crosshairVerts)[0] = osg::Vec3(-width / 2.0f, 0, -height / 2.0f);
	(*crosshairVerts)[1] = osg::Vec3(width / 2.0f, 0, -height / 2.0f);
	(*crosshairVerts)[2] = osg::Vec3(width / 2.0f, 0, height / 2.0f);
	(*crosshairVerts)[3] = osg::Vec3(-width / 2.0f, 0, height / 2.0f);

	billboardQuad->setVertexArray(crosshairVerts);

	osg::Vec2Array* crosshairTexCoords = new osg::Vec2Array(4);
	(*crosshairTexCoords)[0].set(0.0f, 0.0f);
	(*crosshairTexCoords)[1].set(1.0f, 0.0f);
	(*crosshairTexCoords)[2].set(1.0f, 1.0f);
	(*crosshairTexCoords)[3].set(0.0f, 1.0f);
	billboardQuad->setTexCoordArray(0, crosshairTexCoords);

	billboardQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	billboardQuad->setStateSet(bbState);

	return billboardQuad;
}

void createBillboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height)
{
	osg::Billboard* theBillboard = new osg::Billboard();
	_theTrans->addChild(theBillboard);

	//Follow cameras up-direction if the billboard is a crosshair.
	if(_image == "textures/crosshair.png")
		theBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
	else
		theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
	osg::Texture2D *billboardTexture = new osg::Texture2D;

	billboardTexture->setImage(osgDB::readImageFile(_image));

	osg::StateSet* billBoardStateSet = new osg::StateSet;
	billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	billBoardStateSet->setTextureAttributeAndModes(0, billboardTexture, osg::StateAttribute::ON);

	billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
	billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

	osg::Drawable* billboardDrawable;
	billboardDrawable = createBillboardDrawable(_scale, billBoardStateSet, _width, _height);

	theBillboard->addDrawable(billboardDrawable, _pos);
}



void createExplosion(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height)
{

	osg::Billboard* theBillboard = new osg::Billboard();
	_theTrans->addChild(theBillboard);

	//Follow cameras up-direction if the billboard is a crosshair.
	if (_image == "textures/crosshair.png")
		theBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
	else
		theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
	osg::Texture2D *billboardTexture = new osg::Texture2D;

	// CREATE ANIMATION SEQUENCE
	osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;


	// set the sequence for playing.
	billboardTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	billboardTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	billboardTexture->setImage(imageSequence.get());

	osg::StateSet* billBoardStateSet = new osg::StateSet;
	billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	billBoardStateSet->setTextureAttributeAndModes(0, billboardTexture, osg::StateAttribute::ON);

	billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
	billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

	osg::Drawable* billboardAnimatable;
	billboardAnimatable = createBillboardDrawable(_scale, billBoardStateSet, _width, _height);

	theBillboard->addDrawable(billboardAnimatable, _pos);


}
*/

//! Function for setting up skybox. Takes a shared pointer to the transform node.
void makeSkyBox(osg::ref_ptr<osg::MatrixTransform> _mNavTrans)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 90000)));  //scene->getBound().radius())));
	geode->setCullingActive(false);
	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	skybox->setEnvironmentMap(0,
		osgDB::readImageFile("textures/skybox_biggerRT.png"), osgDB::readImageFile("textures/skybox_biggerLF.png"),
		osgDB::readImageFile("textures/skybox_biggerDN.png"), osgDB::readImageFile("textures/skybox_biggerUP.png"),
		osgDB::readImageFile("textures/skybox_biggerFT.png"), osgDB::readImageFile("textures/skybox_biggerBK.png"));
	skybox->addChild(geode.get());
	_mNavTrans->addChild(skybox);
}