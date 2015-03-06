#include "Projectile.h"

Projectile::Projectile(std::string _name, osg::Vec3f _pos, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _dmg, float _vel)
{
	initTransform();
	setVel(_vel);
	setDir(osg::Vec3f(0.0f, 0.0f, 1.0f));
	setColRad(1.0f);
	translate(_pos);
	//scale(0.1f);
	setName(_name);
	setDescr((std::string)("bangbang"));
	_scene->addChild(getTrans());

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

	return *this;
}