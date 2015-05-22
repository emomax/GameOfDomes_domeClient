#include "GameLight.h"

GameLight::GameLight(osg::Vec3f _pos, osg::Vec3f _color, osg::ref_ptr<osg::MatrixTransform> _scene, osg::ref_ptr<osg::Group> _rootNode, float _strength, float _attenuation, std::string attenuationType, bool _redPulse, bool _greenPulse, bool _bluePulse, int _lightIndex)
{
	initTransform();
	//translate(_pos);
	pos = _pos;
	red = _color.x();
	green = _color.y();
	blue = _color.z();
	attenuation = _attenuation;

	theLight = new osg::Light;
	theLightSource = new osg::LightSource;

	theLight->setLightNum(_lightIndex);
	theLight->setPosition(osg::Vec4(_pos, 1.0f));	//We use a transform to translate the light
	theLight->setAmbient(osg::Vec4(red * strength / 10, green * strength / 10, blue * strength / 10, 1.0f));
	theLight->setDiffuse(osg::Vec4(red * strength * 2, green * strength *2, blue * strength * 2, 1.0f));
	theLight->setSpecular(osg::Vec4(0.1f, 0.1f, 0.1f, 1.0f));
	if (attenuationType == "Constant")
		theLight->setConstantAttenuation(attenuation);
	else {
		//theLight->setConstantAttenuation(attenuation);
		theLight->setLinearAttenuation(attenuation);
	}

	theLightSource->setLight(theLight);	//Static light
	theLightSource->setLocalStateSetModes(osg::StateAttribute::ON);
	theLightSource->setStateSetModes(*(_rootNode->getOrCreateStateSet()), osg::StateAttribute::ON);

	_scene->addChild(transform);
	transform->addChild(theLightSource);
}

void GameLight::initTransform()
{
	transform = new osg::MatrixTransform();
	transform->setMatrix(osg::Matrix::identity());
}

void GameLight::translate(osg::Vec3f _t)
{
	pos = pos + _t;
	transform->setMatrix(osg::Matrix::identity());
	transform->postMult(osg::Matrix::translate(pos));
}

GameLight GameLight::operator=(GameLight _l)
{
	setTrans(_l.getTrans());

	red = _l.red;
	green = _l.green;
	blue = _l.blue;
	redPulse = _l.redPulse;
	greenPulse = _l.greenPulse;
	bluePulse = _l.bluePulse;
	strength = _l.strength;
	attenuation = _l.attenuation;
	theLight = _l.theLight;
	theLightSource = _l.theLightSource;

	return *this;
}