#include "Includes.h"

osg::Drawable* createSquare(const osg::Vec3& corner, const osg::Vec3& width, const osg::Vec3& height, osg::Image* image = NULL)
{
	// set up the Geometry.
	osg::Geometry* geom = new osg::Geometry;

	osg::Vec3Array* coords = new osg::Vec3Array(4);
	(*coords)[0] = corner;
	(*coords)[1] = corner + width;
	(*coords)[2] = corner + width + height;
	(*coords)[3] = corner + height;


	geom->setVertexArray(coords);

	osg::Vec3Array* norms = new osg::Vec3Array(1);
	(*norms)[0] = width^height;
	(*norms)[0].normalize();

	geom->setNormalArray(norms, osg::Array::BIND_OVERALL);

	osg::Vec2Array* tcoords = new osg::Vec2Array(4);
	(*tcoords)[0].set(0.0f, 0.0f);
	(*tcoords)[1].set(1.0f, 0.0f);
	(*tcoords)[2].set(1.0f, 1.0f);
	(*tcoords)[3].set(0.0f, 1.0f);
	geom->setTexCoordArray(0, tcoords);

	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

	if (image)
	{
		osg::StateSet* stateset = new osg::StateSet;
		osg::Texture2D* texture = new osg::Texture2D;
		texture->setImage(image);
		stateset->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
		geom->setStateSet(stateset);
	}

	return geom;
}

//Takes the matrix transform it should belong to and returns the rootnode for the scene
osg::Node* createModel(osg::ref_ptr<osg::MatrixTransform> _theTrans, osg::ref_ptr<osg::Group> _theRoot)
{
	// add the drawable into a single geode to be shared...
	osg::Billboard* center = new osg::Billboard();
	osg::Node* textureNode = NULL;
	center->setMode(osg::Billboard::POINT_ROT_EYE);
	center->addDrawable(
		createSquare(osg::Vec3(-0.5f, 0.0f, -0.5f), osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f), osgDB::readImageFile("textures/crosshair.png")),
		osg::Vec3(0.0f, 4.0f, 0.0f));


	if (!(center))
	{
		std::cout << "Couldn't load models, quitting." << std::endl;
	}


	_theTrans->addChild(center);

	return _theRoot;
}