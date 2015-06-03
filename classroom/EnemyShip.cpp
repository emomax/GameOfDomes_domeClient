#include "EnemyShip.h"


EnemyShip::EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, int _hp, int _id)
{
	initTransform();
	setVel(0.0);
	setDir(osg::Vec3f(0.0f, 0.0f, 1.0f));	//Not used
	setOrientation(osg::Quat(0.0f, 0.0f, 1.0f, 0.0f));
	setColRad(_colRad);
	setHP(_hp);
	translate(_pos);
	setName(_name);
	setDescr((std::string)("hej"));
	_scene->addChild(getTrans());
	setID(_id);
	setModel(_model);

	attackCooldown = 10;
	homingMissileAttackCooldown = 40;
}

void EnemyShip::updateAI(osg::Vec3f _playerPos, std::list<Projectile>& _missiles, osg::ref_ptr<osg::MatrixTransform> _mSceneTrans, float _dt)
{
	//std::cout << "enemypos = " << getPos().x() << ", " << getPos().y() << ", " << getPos().z() << std::endl;

	osg::Vec3f diffVec = _playerPos - getPos();
	osg::Quat tempQuat = getOrientation();
	tempQuat.makeRotate(getDir(), diffVec);
	setDir(diffVec / diffVec.length());
	
	if (diffVec.length() > 1000.0f)
		translate(getDir()*5.0f);
	else {

	}
	
	rotate(tempQuat);

	tempQuat = getOrientation();
	osg::Quat tempQuat2;
	tempQuat2.makeRotate(tempQuat * osg::Vec3f(0.0, 1.0, 0.0), getDir());
	tempQuat = tempQuat * tempQuat2;
	
	if (attackCooldown <= 0.0) {
		std::cout << "Enemy Laser!" << std::endl;
		_missiles.push_back(Projectile((std::string)("Laser"), getPos() + tempQuat * osg::Vec3f(0.0, 450.0, 0.0), getDir(), tempQuat, (std::string)("models/skottg.ive"), _mSceneTrans, 50, 4000, false));
		attackCooldown = 8.0;
	}
	else
		attackCooldown -= _dt;
}