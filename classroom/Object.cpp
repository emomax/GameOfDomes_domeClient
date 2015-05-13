#include "skybox.h"

#include "Object.h"

void Object::initTransform()
{
	transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::identity());
}

void Object::translate(osg::Vec3f _t)
{
	pos = pos + _t;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
	//if (name == "Asteroid") transform->postMult(osg::Matrix::scale(osg::Vec3f(2.5, 2.5, 2.1)));
}

void Object::rotate(osg::Quat _q)
{
	orientation = orientation * _q;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::rotate(orientation));
	transform->postMult(osg::Matrix::translate(pos));
	//if(name == "Asteroid") transform->postMult(osg::Matrix::scale(osg::Vec3f(2.5, 2.5, 2.1)));
}
