
#include "Player.h"

Player::Player(std::string _name, osg::Vec3f _pos, float _colRad, int _hp, osg::ref_ptr<osg::MatrixTransform> _scene)
{
	initTransform();
	rigidBodyRadius = _colRad;
	pos = _pos;
	setName(_name);
	setHP(_hp);
	maxHp = hp;

	_scene->addChild(playerTransform);
	playerTransform->postMult(osg::Matrix::translate(pos));

	bridgeTransform->postMult(osg::Matrix::rotate(PI + PI / 4.0, 1.0, 0.0, 0.0));
	bridgeTransform->postMult(osg::Matrix::translate(0.0f, 100.0f, 0.0f));

	bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, 100000, (std::string)("models/kurvbrygga.ive"), bridgeTransform, 100000);
}

void Player::initTransform()
{
	playerTransform = new osg::MatrixTransform();
	bridgeTransform = new osg::MatrixTransform();
	gunnerTransform = new osg::MatrixTransform();
	
	playerTransform->setMatrix(osg::Matrix::identity());
	bridgeTransform->setMatrix(osg::Matrix::identity());
	gunnerTransform->setMatrix(osg::Matrix::identity());

	playerTransform->addChild(bridgeTransform);
	playerTransform->addChild(gunnerTransform);
}

void Player::rotateGunnerTrans(osg::Quat _q)
{
	gunnerTransform->setMatrix(osg::Matrix::identity());
	gunnerTransform->postMult(osg::Matrix::rotate(_q));
}

void Player::resetPlayer()
{
	bridge.removeChildModel(bridge.getModel());
	playerTransform->removeChildren(0, playerTransform->getNumChildren());
	bridgeTransform->removeChildren(0, bridgeTransform->getNumChildren());
	gunnerTransform->removeChildren(0, gunnerTransform->getNumChildren());
}

Player Player::operator=(Player _g)
{
	playerTransform = _g.playerTransform;
	bridgeTransform = _g.bridgeTransform;
	gunnerTransform = _g.gunnerTransform;

	pos = _g.pos;
	hp = _g.hp;
	bridge = _g.bridge;
	hp = _g.hp;
	rigidBodyRadius = _g.rigidBodyRadius;
	scale = _g.scale;
	name = _g.name;

	return *this;
}


