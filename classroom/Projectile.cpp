#include "Projectile.h"

Projectile::Projectile(std::string _name, osg::Vec3f _pos, osg::Vec3f _dir, osg::Quat _orientation, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _dmg, float _vel)
{
	initTransform();
	setVel(-_vel);
	setDir(-_dir);
	setColRad(1.0f);
	translate(_pos);
	setName(_name);
	setDescr((std::string)("bangbang"));
	_scene->addChild(getTrans());

	//_orientation = osg::Quat(PI / 2, 1, 0, 0) * _orientation;
	getTrans()->postMult(osg::Matrix::rotate(_orientation));
	damage = _dmg;
	lifeTime = 5.0f;

	setModel(_model);
}

void Projectile::translate(osg::Vec3f _t)
{
	getTrans()->postMult(osg::Matrix::translate(_t));
	pos = pos + _t;
	std::cout << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
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

	return *this;
}