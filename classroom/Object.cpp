#include "skybox.h"

#include "Object.h"

void Object::initTransform()
{
	transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::identity());
}

void Object::translate(osg::Vec3f _t)
{
	transform->postMult(osg::Matrix::translate(_t));
	pos = pos + _t;
}