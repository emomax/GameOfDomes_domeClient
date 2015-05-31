#include "Billboard.h"


Billboard::Billboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height,std::string _name)
{
	name = _name;
	width = _width;// _width;
	height = _height;// _height;

	if (_name == "Explosion") {
		lifeTime = 30.0f;
		//createExplosion(_scale, _pos, _theTrans, _width, _height);
	}
	else 
	{
		if (_name == "EnemyShield")
			lifeTime = 10.0f;

		theBillboard = new osg::Billboard();
		_theTrans->addChild(theBillboard);
		
		//Follow cameras up-direction if the billboard is a crosshair.
		if (_image == "textures/crosshair.png")
			theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
		else
			theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
		//theTexture = new osg::Texture2D;

		theImage = osgDB::readImageFile(_image);

		theRect = new osg::TextureRectangle(theImage);
		//theRect->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		//theRect->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		theRect->setTextureSize(width, height);
		theRect->setResizeNonPowerOfTwoHint(false);
		texMat = new osg::TexMat;
		texMat->setScaleByTextureRectangleSize(true);

		//theRect->setImage(theImage);

		osg::StateSet* billBoardStateSet = new osg::StateSet;
		billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		billBoardStateSet->setTextureAttributeAndModes(0, theRect, osg::StateAttribute::ON);
		billBoardStateSet->setTextureAttributeAndModes(0, texMat, osg::StateAttribute::ON);

		billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
		billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

		osg::Drawable* billboardDrawable;
		billboardDrawable = createDrawable(_scale, billBoardStateSet, _width, _height);

		theBillboard->addDrawable(billboardDrawable, _pos);

		//theTexture = new osg::TextureRectangle(theImage);
	}
}

Billboard::Billboard(float _scale, osg::Vec3f _pos, osg::ref_ptr<osg::ImageSequence> _sequence, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height, std::string _name)
{
	lifeTime = 10.0f; //Standard lifeTime if nothing else is set

	if (_name == "Explosion")
		lifeTime = 1.2f;
	if (_name == "EnemyShield")
		lifeTime = 0.3f;
	

	theBillboard = new osg::Billboard();
	_theTrans->addChild(theBillboard);


	theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
	//osg::Texture2D *billboardTexture
	theTexture = new osg::Texture2D;

	_sequence->rewind();
	_sequence->setLoopingMode(osg::ImageStream::NO_LOOPING);
	_sequence->play();

	// set the sequence for playing.
	theTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	theTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	theTexture->setImage(_sequence.get());

	osg::StateSet* billBoardStateSet = new osg::StateSet;
	billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	billBoardStateSet->setTextureAttributeAndModes(0, theTexture, osg::StateAttribute::ON);

	billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
	billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

	osg::Drawable* billboardAnimatable;
	billboardAnimatable = createDrawable(_scale, billBoardStateSet, _width, _height);

	theBillboard->addDrawable(billboardAnimatable, _pos);

}


osg::Drawable* Billboard::createDrawable(const float & _scale, osg::StateSet* _bbState, float _width, float _height)
{
	_width *= _scale;
	_height *= _scale;

	osg::Geometry* billboardQuad = new osg::Geometry;

	osg::Vec3Array* crosshairVerts = new osg::Vec3Array(4);
	(*crosshairVerts)[0] = osg::Vec3(-_width / 2.0f, 0, -_height / 2.0f);
	(*crosshairVerts)[1] = osg::Vec3(_width / 2.0f, 0, -_height / 2.0f);
	(*crosshairVerts)[2] = osg::Vec3(_width / 2.0f, 0, _height / 2.0f);
	(*crosshairVerts)[3] = osg::Vec3(-_width / 2.0f, 0, _height / 2.0f);

	billboardQuad->setVertexArray(crosshairVerts);

	osg::Vec2Array* crosshairTexCoords = new osg::Vec2Array(4);
	(*crosshairTexCoords)[0].set(0.0f, 0.0f);
	(*crosshairTexCoords)[1].set(1.0f, 0.0f);
	(*crosshairTexCoords)[2].set(1.0f, 1.0f);
	(*crosshairTexCoords)[3].set(0.0f, 1.0f);
	billboardQuad->setTexCoordArray(0, crosshairTexCoords);

	osg::Vec4Array* colors = new osg::Vec4Array(1);
	(*colors)[0].set(1.0f, 1.0f, 1.0f, 1.0f);
	billboardQuad->setColorArray(colors, osg::Array::BIND_OVERALL);

	billboardQuad->setUseDisplayList(false);

	billboardQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	billboardQuad->setStateSet(_bbState);

	return billboardQuad;
}



bool Billboard::isTimed()
{
	if (lifeTime == -1.0f)
		return false;

	return true;
}

void Billboard::reScale(float _scaleX, float _scaleY)
{
	texMat->setMatrix(osg::Matrix::inverse(osg::Matrix::scale(_scaleX, _scaleY, _scaleX)));
}