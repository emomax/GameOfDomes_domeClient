#include "EnemyShip.h"


EnemyShip::EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _hp, int _id)
{
	initTransform();
	setVel(0.0);
	setDir(osg::Vec3f(0.0f, 0.0f, 1.0f));
	setOrientation(osg::Quat(0.0f, 0.0f, 1.0f, 0.0f));
	setColRad(_colRad);
	setHp(_hp);
	translate(_pos);
	setName(_name);
	setDescr((std::string)("hej"));
	_scene->addChild(getTrans());
	setID(_id);
	setModel(_model);
}

void EnemyShip::updateAI(osg::Vec3f _playerPos)
{
	osg::Vec3f diffVec = _playerPos - getPos();
	osg::Quat tempQuat;
	tempQuat.makeRotate(getDir(), diffVec);
	setDir(diffVec);
	
	translate(diffVec / diffVec.length()*5.0f);
	rotate(tempQuat);
}