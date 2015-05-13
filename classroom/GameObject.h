#pragma once

#include "Object.h"
#include "Includes.h"
//#include <osgDB/ReadFile>
//#include <osg/MatrixTransform>
//#include <osg/ComputeBoundsVisitor>
//
//#include <osg/Depth>
//#include <osg/TexGen>
//#include <osg/TextureCubeMap>
//#include <osg/ShapeDrawable>
//#include <osg/Geode>
//#include <osgDB/ReadFile>
//#include <osgUtil/CullVisitor>

class GameObject : public Object
{
public:
	GameObject()
	{
		rigidBodyRadius = 0.0;
		initTransform();
	}

	GameObject(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, int _id);

	osg::ref_ptr<osg::Node> getModel(){ return model; }
	int getID() { return index; }
	float getColRad() { return rigidBodyRadius; }
	//getModel();

	void setID(int _id) { index = _id; }
	void setColRad(float _c) { rigidBodyRadius = _c; }
	void setModel(osg::ref_ptr<osg::Node> _m) { model = _m; }
	void setModel(std::string _fileName);

	GameObject operator=(GameObject _g);
	bool operator==(GameObject _g) { if (index == _g.index) return true; else return false; }

	virtual ~GameObject() {}

	virtual void updateAI(osg::Vec3f _playerPos) { return; };

private:
	float rigidBodyRadius;
	int hp;
	osg::ref_ptr<osg::Node> model;

	//Unique index for all objects in the scene. Used for comparison operator.
	int index;
};
