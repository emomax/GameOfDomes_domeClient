#include "Utilities.h"

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

		//Create the crosshair and set to be child of gunnerTrans
		_billList.push_back(Billboard(0.5, osg::Vec3f(0, 2.5, 0), "textures/crosshair.png", _player.getGunnerTrans(), 1.0, 1.0, "Crosshair"));

		//Create the healthbar and set to be child of healthbarTrans
		_billList.push_back(Billboard(1.0, osg::Vec3f(0, 2.5, 0), "textures/healthbar_full.png", _player.getHealthbarTrans(), 3, 3 * (88.0 / 948), "Healthbar"));
		_billList.push_back(Billboard(1.0, osg::Vec3f(0, 2.51, 0), "textures/healthbar_empty.png", _player.getHealthbarTrans(), 3, 3 * (88.0 / 948), "Healthbar_empty"));


		//Create a billboard representing the sun
		_billList.push_back(Billboard(80000.0, osg::Vec3f(80000, 0, 0), "textures/sol.png", _mSceneTrans, 1.0, 1.0, "Sun"));
		_billList.push_back(Billboard(30000.0, osg::Vec3f(50000, 50000, 0), "textures/planet_lila.png", _mSceneTrans, 1.0, 1.0, "Planet1"));
		_billList.push_back(Billboard(30000.0, osg::Vec3f(-50000, -50000, 0), "textures/planet_rings_moon.png", _mSceneTrans, 1.0, 1.0, "Planet2"));
		_billList.push_back(Billboard(30000.0, osg::Vec3f(0, 80000, 0), "textures/Planet_three_moons.png", _mSceneTrans, 1.0, 1.0, "Planet3"));
		_billList.push_back(Billboard(30000.0, osg::Vec3f(15000, -70000, 0), "textures/Planet_vit.png", _mSceneTrans, 1.0, 1.0, "Planet4"));

		//Fill scene with 50 asteroids.
		for (int i = 0; i < _asteroidAmount; i++)
		{
			//Because the values need to be synced, we can't use standard random values since they depend on a local random seed.
			//Modulus operator retuns an int, so we need to re-cast it as a float.

			int rand1 = 50000 - (_randomSeed * 3571 + 997) % 100000;  //generate random value between -5000 and 5000
			int rand2 = 50000 - ((50000 + rand1) * 3571 + 997) % 100000; //generate new random value between -5000 and 5000
			int rand3 = 50000 - ((50000 + rand2) * 3571 + 997) % 100000; //Prime numbers are used to avoid repetitions.

			_randomSeed = 50000 + rand3;

			if (i % 3 == 0)
				_objList.push_back(new GameObject((std::string)("Asteroid"), osg::Vec3f(rand1, rand2, rand3), 2500.0f, 500, (std::string)("models/asteroid_5meter.ive"), _mSceneTrans, _objIndex++));
			//else
			//	_objList.push_back(new GameObject((std::string)("Asteroid"), osg::Vec3f(rand1, rand2, rand3), 2500.0f, 500, (std::string)("models/asteroid_1m.obj"), _mSceneTrans, _objIndex++));

			//_objList.back()->setScale(randScale);						//pos variable need to take the scale into account. Save for later.
			_objList.back()->rotate(osg::Quat(_randomSeed, rand1, rand2, rand3));
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

//Returns a random value between 0-100
int randomValue(int _randomSeed)
{
	std::cout << "random value is: " << 100 - (_randomSeed * 3571 + 997) % 100 << " and randomseed is: " << _randomSeed << "\n";
	return 100 - (_randomSeed * 3571 + 997) % 100;

}


//! Function for setting up skybox. Takes a shared pointer to the transform node.
void makeSkyBox(osg::ref_ptr<osg::MatrixTransform> _mNavTrans)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(new osg::ShapeDrawable(
		new osg::Sphere(osg::Vec3(), 500000)));  //scene->getBound().radius())));
	geode->setCullingActive(false);
	osg::ref_ptr<SkyBox> skybox = new SkyBox;
	skybox->getOrCreateStateSet()->setTextureAttributeAndModes(0, new osg::TexGen);
	cout << "skybox started loading...\n";
	skybox->setEnvironmentMap(0,
		osgDB::readImageFile("textures/skybox_biggerRT.png"), osgDB::readImageFile("textures/skybox_biggerLF.png"),
		osgDB::readImageFile("textures/skybox_biggerDN.png"), osgDB::readImageFile("textures/skybox_biggerUP.png"),
		osgDB::readImageFile("textures/skybox_biggerFT.png"), osgDB::readImageFile("textures/skybox_biggerBK.png"));
	cout << "skybox finished loading!\n";
	skybox->addChild(geode.get());
	_mNavTrans->addChild(skybox);
}