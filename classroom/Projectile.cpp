#include "Projectile.h"

Projectile::Projectile(std::string _name, osg::Vec3f _pos, osg::Vec3f _dir, osg::Quat _orientation, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _dmg, float _vel, bool _playerOwned)
{
	initTransform();
	setVel(_vel);
	setDir(_dir);
	setOrientation(_orientation);
	setColRad(50.0f);
	translate(_pos);
	setName(_name);
	setDescr((std::string)("bangbang"));
	_scene->addChild(getTrans());
	damage = _dmg;
	lifeTime = 3.0f;
	playerOwned = _playerOwned;

	setModel(_model);
}

Projectile Projectile::operator=(Projectile _p)
{
	setTrans(_p.getTrans());
	setDir(_p.getDir());
	setVel(_p.getVel());
	setName(_p.getName());
	setDescr(_p.getDescr());
	setColRad(_p.getColRad());
	setModel(_p.getModel());

	damage = _p.damage;
	lifeTime = _p.lifeTime;
	playerOwned = _p.playerOwned;

	return *this;
}