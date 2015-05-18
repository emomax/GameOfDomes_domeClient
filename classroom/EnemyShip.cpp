#include "EnemyShip.h"


EnemyShip::EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, int _hp, int _id)
{
	initTransform();
	setVel(0.0);
	setDir(osg::Vec3f(0.0f, 0.0f, 1.0f));	//Not used
	upDir = osg::Vec3f(0.0f, 0.0, 1.0f);
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
	osg::Vec3f diffVec = _playerPos - getPos();
	osg::Quat tempQuat = getOrientation();
	tempQuat.makeRotate(getDir(), diffVec);
	setDir(diffVec / diffVec.length());
	
	translate(getDir()*5.0f);
	rotate(tempQuat);

	tempQuat = getOrientation();
	upDir = tempQuat * osg::Vec3f(0.0, 1.0, 0.0);
	tempQuat.makeRotate(upDir, -getDir());
	
	if (attackCooldown <= 0.0) {
		_missiles.push_back(Projectile((std::string)("Laser"), getPos() + tempQuat * osg::Vec3f(0.0, 300.0, 0.0), getDir(), tempQuat, (std::string)("models/skott20m.obj"), _mSceneTrans, 50, 4000));
		attackCooldown = 8.0;
	}
	else
		attackCooldown -= _dt;
}