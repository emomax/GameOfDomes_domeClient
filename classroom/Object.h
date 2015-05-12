#pragma once
#include "Includes.h"

class Object : public osg::Transform
{
public:
	float getVel() { return velocity; }
	osg::Vec3f getDir() { return direction; }
	osg::Quat getOrientation() { return orientation; }
	osg::Vec3f getPos() { return pos; }
	osg::ref_ptr<osg::MatrixTransform> getTrans() { return transform; }

	std::string getName() { return name; }
	std::string getDescr() { return description; }

	void initTransform();
	void setVel(float _v) { velocity = _v; }
	void setDir(osg::Vec3f _d) { direction = _d; }
	void setOrientation(osg::Quat _q) { orientation = _q; }
	void setPos(osg::Vec3f _pos) { pos = _pos; }	//setPos is rarely used directly since translate also sets a new position
	void setScale(float _s) { scale = _s; }
	void translate(osg::Vec3f _t);
	void rotate(osg::Quat _q);
	void setTrans(osg::ref_ptr<osg::MatrixTransform> _t) { transform = _t; }

	void addChildModel(osg::ref_ptr<osg::Node> _m) { transform->addChild(_m); }
	void removeChildModel(osg::ref_ptr<osg::Node> _m) { transform->removeChild(_m); }

	void setName(std::string _n) { name = _n; }
	void setDescr(std::string _d) { description = _d; }

	virtual ~Object() {}

private:
		osg::ref_ptr<osg::MatrixTransform> transform;

		osg::Vec3f direction;
		osg::Quat orientation;
		osg::Vec3f pos = osg::Vec3f(0,0,0);
		float velocity;
		float scale = 1.0;

		//The name variable is currently used for determining the type of the object, so it is important to follow 
		//naming conventions. The current name types are: (case sensitive)
		//	Asteroid
		//	Enemy
		std::string name;
		std::string description;
};
