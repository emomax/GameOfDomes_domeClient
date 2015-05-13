#pragma once
#include "Includes.h"
#include "GameObject.h"
//#include "Utilities.h"

/*
As of now, the Player implementation is semi-hardcoded in the main.cpp file. In the end its supposed to be inserted into this class.
Currently the player class takes care of:
- Player related matrix transforms
- Collision check and hp calculation
*/

class Player : public osg::Transform
{
public:
	Player()
	{
		rigidBodyRadius = 0.0;
		initTransform();
	}
	Player(std::string _name, osg::Vec3f _pos, float _colRad, osg::ref_ptr<osg::MatrixTransform> _scene);

	//float getVel() { return velocity; }
	//osg::Vec3f getDir() { return direction; }
	//osg::Quat getOrientation() { return orientation; }
	osg::Vec3f getPos() { return pos; }
	int getHP() { return hp; }
	float getColRad() { return rigidBodyRadius; }
	osg::ref_ptr<osg::MatrixTransform> getPlayerTrans() { return playerTransform; }
	osg::ref_ptr<osg::MatrixTransform> getBridgeTrans() { return bridgeTransform; }
	osg::ref_ptr<osg::MatrixTransform> getGunnerTrans() { return gunnerTransform; }
	std::string getName() { return name; }

	void initTransform();
	//void setVel(float _v) { velocity = _v; }
	void setHP(float _hp) { hp = _hp; }
	//void setDir(osg::Vec3f _d) { direction = _d; }
	//void setOrientation(osg::Quat _q) { orientation = _q; }
	void setPos(osg::Vec3f _pos) { pos = _pos; }
	void setScale(float _s) { scale = _s; }
	//void translate(osg::Vec3f _t);
	//void rotate(osg::Quat _q);

	void setPlayerTrans(osg::ref_ptr<osg::MatrixTransform> _t) { playerTransform = _t; }
	void setBridgeTrans(osg::ref_ptr<osg::MatrixTransform> _t) { bridgeTransform = _t; }
	void rotateGunnerTrans(osg::Quat _q);


	void setName(std::string _n) { name = _n; }

	virtual ~Player() {}

	Player operator=(Player _g);

private:
		osg::ref_ptr<osg::MatrixTransform> playerTransform;
		osg::ref_ptr<osg::MatrixTransform> bridgeTransform;
		osg::ref_ptr<osg::MatrixTransform> gunnerTransform;
		
		osg::Vec3f pos = osg::Vec3f(0,0,0);

		GameObject bridge;

		int hp;
		float rigidBodyRadius;
		float scale = 1.0;

		//The name variable is currently not used. Later it may be used for multiplayer games etc.
		std::string name;
};
