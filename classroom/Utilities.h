/*
Utilities.h contains useful functions that the program uses.
*/

#include "NetworkManager.h"
#include "SoundManager.h"

#include "SkyBox.h"
#include "Includes.h"
#include "Projectile.h"
#include "Player.h"
#include "Billboard.h"


//Function declarations
void setGameState(int _state, int& _objIndex, std::list<GameObject*>& _objList, std::list<Billboard>& _billList, 
	Player& _player, osg::ref_ptr<osg::MatrixTransform> _mNavTrans, osg::ref_ptr<osg::MatrixTransform> _mRootTrans, osg::ref_ptr<osg::MatrixTransform> _mSceneTrans,
	osg::ref_ptr<osg::MatrixTransform> _mWelcomeTrans, SoundManager& _soundManager, int _randomSeed, int _asteroidAmount, bool _isMaster, int _bridgeModel);

void makeSkyBox(osg::ref_ptr<osg::MatrixTransform> _mNavTrans);

int randomValue(int _randomSeed);

bool isPowerupIcon(list<Billboard>::iterator _b);