#pragma once
#include "Includes.h"

class Powerup : public osg::Transform
{
public:
	Powerup(){ initTransform(); }
	Powerup(std::string _name, osg::Vec3f _pos, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene);

	osg::ref_ptr<osg::Node> getModel(){ return model; }
	osg::Quat getOrientation() { return orientation; }
	osg::Vec3f getPos() { return pos; }
	osg::ref_ptr<osg::MatrixTransform> getTrans() { return transform; }

	std::string getName() { return name; }
	std::string getDescr() { return description; }

	void initTransform();
	void setOrientation(osg::Quat _q) { orientation = _q; }
	void setPos(osg::Vec3f _pos) { pos = _pos; }	//setPos is rarely used directly since translate also sets a new position
	void translate(osg::Vec3f _t);
	void rotate(osg::Quat _q);
	void setTrans(osg::ref_ptr<osg::MatrixTransform> _t) { transform = _t; }

	float getColRad() { return rigidBodyRadius; }
	void setColRad(float _c) { rigidBodyRadius = _c; }

	void addChildModel(osg::ref_ptr<osg::Node> _m) { transform->addChild(_m); }
	void removeChildModel(osg::ref_ptr<osg::Node> _m) { transform->removeChild(_m); }

	void setName(std::string _n) { name = _n; }
	void setDescr(std::string _d) { description = _d; }

	void setModel(osg::ref_ptr<osg::Node> _m) { model = _m; }
	void setModel(std::string _fileName);

	virtual ~Powerup() {}

private:
		osg::ref_ptr<osg::MatrixTransform> transform;
		osg::ref_ptr<osg::Node> model;

		osg::Quat orientation = osg::Quat(0.0f, 1.0f, 0.0f, 0.0f);
		osg::Vec3f pos = osg::Vec3f(0,0,0);
		float rigidBodyRadius = 200;

		//The name variable is currently used for determining the type of the powerup, so it is important to follow 
		//naming conventions. The current name types are: (case sensitive)
		//	HealthPowerup
		//	SkottPowerup
		//	ShieldPowerup
		std::string name;
		std::string description;
};
