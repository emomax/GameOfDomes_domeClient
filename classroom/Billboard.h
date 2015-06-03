#pragma once

#include "Includes.h"

class Billboard : public osg::Transform
{
public:
	Billboard();

	Billboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height, std::string _name);
	Billboard(float _scale, osg::Vec3f _pos, osg::ref_ptr<osg::ImageSequence> _sequence, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height, std::string _name);

	bool isTimed();
	float getLifeTime() { return lifeTime; }
	void setLifeTime(float _t) { lifeTime = _t; }
	std::string getName() { return name; }

	// Max testing rescaling
	void reScale(float _scaleX, float _scaleY);
	
	void removeBillboard() { theBillboard->removeDrawables(0, theBillboard->getNumDrawables());  }

	osg::Billboard* getBillboard() { return theBillboard; }

	Billboard operator=(Billboard _b);

	virtual ~Billboard() {}

private:
	float lifeTime = -1;	//-1 means the billboard will not be time-dependent

	osg::Billboard* theBillboard;
	osg::Texture2D* theTexture;
	osg::TextureRectangle* theRect;
	osg::TexMat* texMat;
	osg::Image* theImage;
	std::string name;
	float width, height;

	osg::Drawable* createDrawable(const float & scale, osg::StateSet* bbState, float width, float height);
	void createExplosion(float _scale, osg::Vec3f _pos, osg::ref_ptr<osg::ImageSequence> _sequence, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height);
};
