#pragma once
#include "Includes.h"

class GameLight : public osg::Transform
{
public:

	GameLight()
	{
		initTransform();
		//theLight = new osg::Light;
		//theLightSource = new osg::LightSource;
	}

	//attenuationType is either 'Constant' or 'Linear'
	GameLight(osg::Vec3f _pos, osg::Vec3f _color, osg::ref_ptr<osg::MatrixTransform> _scene, osg::ref_ptr<osg::Group> _rootNode ,float _strength, float _attenuation, std::string attenuationType, bool _redPulse, bool _greenPulse, bool _bluePulse, int _lightIndex);

	void initTransform();

	osg::Vec3f getPos() { return pos; }

	void setColor(float _red, float _green, float _blue);

	osg::ref_ptr<osg::LightSource> getLight() { return theLightSource; }

	void setPos(osg::Vec3f _pos) { pos = _pos; }	//setPos is rarely used directly since translate also sets a new position
	void translate(osg::Vec3f _t);

	osg::ref_ptr<osg::MatrixTransform> getTrans() { return transform; }
	void setTrans(osg::ref_ptr<osg::MatrixTransform> _t) { transform = _t; }

	void removeChildLight(osg::LightSource* _l) { transform->removeChild(_l); }

	GameLight operator=(GameLight _p);

	virtual ~GameLight() {}

private:
		osg::ref_ptr<osg::MatrixTransform> transform;

		osg::Vec3f pos = osg::Vec3f(0,0,0);
		float red = 0.0;
		float green = 0.0;
		float blue = 0.0;
		float strength = 1.0;
		float attenuation = 0.05;

		osg::Light *theLight;
		osg::LightSource* theLightSource;

		bool redPulse = false;
		bool greenPulse = false;
		bool bluePulse = false;
};
