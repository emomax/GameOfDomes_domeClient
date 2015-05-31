
#include "Player.h"

Player::Player(std::string _name, osg::Vec3f _pos, float _colRad, int _hp, osg::ref_ptr<osg::MatrixTransform> _scene, int _bridgeModel)
{
	initTransform();
	rigidBodyRadius = _colRad;
	pos = _pos;
	setName(_name);
	setHP(_hp);
	setMaxHP(_hp);

	_scene->addChild(playerTransform);
	playerTransform->postMult(osg::Matrix::translate(pos));


	healthbarTransform->postMult(osg::Matrix::rotate(-PI / 8, 1.0f, 0.0f, 0.0f));

	bridgeTransform->postMult(osg::Matrix::rotate(PI + PI / 4.0, 1.0, 0.0, 0.0));
	//bridgeTransform->postMult(osg::Matrix::scale(0.1f, 0.1f, 0.1f));

	if (_bridgeModel == 1) {
		bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, 100000, (std::string)("models/kurvbrygga_lasselagom.ive"), bridgeTransform, 100000);
		bridgeTransform->postMult(osg::Matrix::translate(0.0f, 3.5f, 0.0f));
	}
	if (_bridgeModel == 2) {
		bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, 100000, (std::string)("models/kurvbrygga_lasselagom.ive"), bridgeTransform, 100000);
		bridgeTransform->postMult(osg::Matrix::translate(0.0f, 2.5f, 0.0f));
	}
	if (_bridgeModel == 3) {
		bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, 100000, (std::string)("models/kurvbrygga_lassesmal.ive"), bridgeTransform, 100000);
		bridgeTransform->postMult(osg::Matrix::translate(0.0f, 3.5f, 0.0f));
	}
	if (_bridgeModel == 4) {
		bridge = GameObject((std::string)("Kommandobryggan"), osg::Vec3f(0, 0, 0), 0, 100000, (std::string)("models/kurvbrygga_lassesmal.ive"), bridgeTransform, 100000);
		bridgeTransform->postMult(osg::Matrix::translate(0.0f, 2.5f, 0.0f));
	}
}

void Player::initTransform()
{
	playerTransform = new osg::MatrixTransform();
	bridgeTransform = new osg::MatrixTransform();
	gunnerTransform = new osg::MatrixTransform();
	healthbarTransform = new osg::MatrixTransform();
	
	playerTransform->setMatrix(osg::Matrix::identity());
	bridgeTransform->setMatrix(osg::Matrix::identity());
	gunnerTransform->setMatrix(osg::Matrix::identity());
	healthbarTransform->setMatrix(osg::Matrix::identity());

	playerTransform->addChild(bridgeTransform);
	playerTransform->addChild(gunnerTransform);
	playerTransform->addChild(healthbarTransform);
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
	healthbarTransform->removeChildren(0, healthbarTransform->getNumChildren());
}

Player Player::operator=(Player _g)
{
	if (playerTransform != nullptr)
		playerTransform = _g.playerTransform;
	if (bridgeTransform != nullptr)
		bridgeTransform = _g.bridgeTransform;
	if (gunnerTransform != nullptr)
		gunnerTransform = _g.gunnerTransform;
	if (healthbarTransform != nullptr)
		healthbarTransform = _g.healthbarTransform;

	pos = _g.pos;
	hp = _g.hp;
	maxHp = _g.maxHp;
	bridge = _g.bridge;
	hp = _g.hp;
	rigidBodyRadius = _g.rigidBodyRadius;
	scale = _g.scale;
	name = _g.name;

	return *this;
}

void Player::reScale(float _scaleX, float _scaleY)
{
	std::cout << "rescaling: " << name << " scalex = " << _scaleX << " scaleY =  " << _scaleY << "\n";
	healthbarTransform->setMatrix(osg::Matrix::inverse(osg::Matrix::scale(_scaleX, _scaleY, _scaleX)));
}
