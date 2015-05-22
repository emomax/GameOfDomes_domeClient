#include "Powerup.h"

Powerup::Powerup(std::string _name, osg::Vec3f _pos, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene)
{
	initTransform();
	rigidBodyRadius = 300;
	translate(_pos);
	setName(_name);
	setDescr((std::string)("hej"));
	_scene->addChild(getTrans());
	setModel(_model);
}
void Powerup::initTransform()
{
	transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::identity());
}

void Powerup::translate(osg::Vec3f _t)
{
	pos = pos + _t;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
}

void Powerup::rotate(osg::Quat _q)
{
	orientation = orientation * _q;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
}

void Powerup::setModel(std::string _fileName)
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