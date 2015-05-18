#include "Billboard.h"


Billboard::Billboard(float _scale, osg::Vec3f _pos, std::string _image, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height,std::string _name)
{

	if (_name == "Explosion") {
		lifeTime = 30.0f;
		createExplosion(_scale, _pos, _theTrans, _width, _height);
	}
	else 
	{
		if (_name == "EnemyShield")
			lifeTime = 10.0f;

		theBillboard = new osg::Billboard();
		_theTrans->addChild(theBillboard);
		
		//Follow cameras up-direction if the billboard is a crosshair.
		if (_image == "textures/crosshair.png")
			theBillboard->setMode(osg::Billboard::POINT_ROT_EYE);
		else
			theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
		osg::Texture2D *billboardTexture = new osg::Texture2D;

		billboardTexture->setImage(osgDB::readImageFile(_image));

		osg::StateSet* billBoardStateSet = new osg::StateSet;
		billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		billBoardStateSet->setTextureAttributeAndModes(0, billboardTexture, osg::StateAttribute::ON);

		billBoardStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		billBoardStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

		// Enable depth test so that an opaque polygon will occlude a transparent one behind it.
		billBoardStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

		osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
		billBoardStateSet->setAttributeAndModes(bf, osg::StateAttribute::ON);

		osg::Drawable* billboardDrawable;
		billboardDrawable = createDrawable(_scale, billBoardStateSet, _width, _height);

		theBillboard->addDrawable(billboardDrawable, _pos);
	}
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

	billboardQuad->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	billboardQuad->setStateSet(_bbState);

	return billboardQuad;
}

void Billboard::createExplosion(float _scale, osg::Vec3f _pos, osg::ref_ptr<osg::MatrixTransform> _theTrans, float _width, float _height)
{

	theBillboard = new osg::Billboard();
	_theTrans->addChild(theBillboard);


	theBillboard->setMode(osg::Billboard::POINT_ROT_WORLD);
	osg::Texture2D *billboardTexture = new osg::Texture2D;

	// CREATE ANIMATION SEQUENCE
	osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;

	imageSequence->setLength(1.2);
	// load images
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00001.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00002.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00003.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00004.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00005.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00006.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00007.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00008.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00009.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00010.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00011.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00012.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00013.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00014.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00015.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00016.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00017.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00018.png"));
	imageSequence->addImage(osgDB::readImageFile("textures/Explosion_00019.png"));

	//imageSequence->setLoopingMode(osg::ImageStream::NO_LOOPING);
	imageSequence->play();

	// set the sequence for playing.
	billboardTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
	billboardTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	billboardTexture->setImage(imageSequence.get());

	osg::StateSet* billBoardStateSet = new osg::StateSet;
	billBoardStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	billBoardStateSet->setTextureAttributeAndModes(0, billboardTexture, osg::StateAttribute::ON);

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

bool Billboard::isTimed()
{
	if (lifeTime == -1.0f)
		return false;

	return true;
}