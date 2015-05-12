#include "GameObject.h"

GameObject::GameObject(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, int _id)
{
	initTransform();
	setVel(0.0);
	setDir(osg::Vec3f(0.0f, 0.0f, 1.0f));
	setOrientation(osg::Quat(0.0f, 0.0f, 0.0f, 1.0f));
	rigidBodyRadius = _colRad;
	translate(_pos);
	setName(_name);
	setDescr((std::string)("hej"));
	_scene->addChild(getTrans());
	setID(_id);
	setModel(_model);
	//setScale(0.001);
}

void GameObject::setModel(std::string _fileName)
{
	model = osgDB::readNodeFile(_fileName);

	if (model.valid())
	{
		addChildModel(model);

		//disable face culling
		model->getOrCreateStateSet()->setMode(GL_CULL_FACE,
			osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		//rigidBodyRadius = bb.radius();
	}
}

GameObject GameObject::operator=(GameObject _g)
{
	setTrans(_g.getTrans());
	setDir(_g.getDir());
	setVel(_g.getVel());
	setName(_g.getName());
	setDescr(_g.getDescr());
	setColRad(_g.getColRad());
	setModel(_g.getModel());
	
	return *this;
}
