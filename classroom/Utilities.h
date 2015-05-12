/*
Utilities.h contains useful functions that the program uses.
*/

#include "Includes.h"

#include "Projectile.h"
#include "NetworkManager.h"
#include "SoundManager.h"


//Function declarations
void createBillboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans);


//Function for changing level. the list containing all objects and the relevant matrix transforms are called as reference. Note that the matrix transforms are pointers.
void setGameState(int _state, int& _objIndex, GameObject _player, GameObject _bridge, std::list<GameObject*>& _objList, osg::ref_ptr<osg::MatrixTransform> _mGunnerTrans,
	osg::ref_ptr<osg::MatrixTransform> _mNavTrans, osg::ref_ptr<osg::MatrixTransform> _mPlayerTrans, osg::ref_ptr<osg::MatrixTransform> _mBridgeTrans,
	osg::ref_ptr<osg::MatrixTransform> _mSceneTrans, SoundManager& _soundManager, int _randomSeed) {

	switch (_state) {
		//Welcome Screen
	case 0: {
				cout << "State set to WELCOME_SCREEN." << endl;
				createBillboard(1.0, osg::Vec3f(0, 3, 0), "textures/dome_startscreen.png", _mGunnerTrans);
				_soundManager.play("mainMenu_music", osg::Vec3f(0.0f, 0.0f, 0.0f));
				_state = 0;
	}
		break;

		//Game Screen
	case 1: {
				cout << "State set to GAME_SCREEN." << endl;

				_mGunnerTrans->removeChildren(0, _mGunnerTrans->getNumChildren());
				//Create the crosshair and set to be child of mGunnerTrans
				createBillboard(0.3, osg::Vec3f(0, 2.5, 0), "textures/crosshair.png", _mGunnerTrans);

				//Create a billboard representing the sun
				createBillboard(1000.0, osg::Vec3f(3000, 3000, 0), "textures/sol.png", _mNavTrans);
				//Add player object and commandbridge model
				_player = GameObject((std::string)("Spelaren"), osg::Vec3f(0, 0, 0), 100.0, (std::string)(""), _mPlayerTrans, _objIndex++);
				_bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, (std::string)("models/kurvbrygga.ive"), _mBridgeTrans, _objIndex++);


				//Fill scene with 50 asteroids.
				for (int i = 0; i < 50; i++)
				{
					int rand1 = 500 - (_randomSeed * 3571 + 997) % 1000;  //generate random value between -5000 and 5000
					int rand2 = 500 - ((500 + rand1) * 3571 + 997) % 1000; //generate new random value between -5000 and 5000
					int rand3 = 500 - ((500 + rand2) * 3571 + 997) % 1000; //Prime numbers are used to avoid repetitions.
					std::cout << rand1 << " " << rand2 << " " << rand3 << std::endl;
					float randScale = 1.2 - (float)(((500 + rand3) * 3571 + 997) % 400) / 1000; //generate random value between 0.8 - 1.2
					//randScale *= 10;

					_randomSeed = 500 + rand3;

					_objList.push_back(new GameObject((std::string)("Asteroid"), osg::Vec3f(rand1, rand2, rand3), 50.0f, (std::string)("models/asteroid_1.ive"), _mSceneTrans, _objIndex++));
					_objList.back()->setScale(randScale);						//pos variable need to take the scale into account. Save for later.
					_objList.back()->rotate(osg::Quat(_randomSeed, rand1, rand2, rand3));
					std::cout << _objList.back()->getName() << _objList.back()->getID() << std::endl;
				}

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
				_soundManager.play("preGame_music", osg::Vec3f(0.0f, 0.0f, 0.0f));
				_state = 3;
	}
		break;
	}

}


//Billboard functions
#pragma once
osg::Drawable* createBillboardDrawable(const float & scale, osg::StateSet* bbState)
{
	float width = 1.5f;
	float height = 3.0f;

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
#pragma once
void createBillboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans)
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
	billboardDrawable = createBillboardDrawable(_scale, billBoardStateSet);

	theBillboard->addDrawable(billboardDrawable, _pos);


}


