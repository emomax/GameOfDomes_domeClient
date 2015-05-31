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
	Player(std::string _name, osg::Vec3f _pos, float _colRad, int _hp, osg::ref_ptr<osg::MatrixTransform> _scene, int _bridgemodel);

	osg::Vec3f getPos() { return pos; }
	float getColRad() { return rigidBodyRadius; }
	osg::ref_ptr<osg::MatrixTransform> getPlayerTrans() { return playerTransform; }
	osg::ref_ptr<osg::MatrixTransform> getBridgeTrans() { return bridgeTransform; }
	osg::ref_ptr<osg::MatrixTransform> getGunnerTrans() { return gunnerTransform; }
	osg::ref_ptr<osg::MatrixTransform> getHealthbarTrans() { return healthbarTransform; }
	std::string getName() { return name; }

	void initTransform();
	void setPos(osg::Vec3f _pos) { pos = _pos; }
	void setScale(float _s) { scale = _s; }
	int getHP() { return hp; }
	int getMaxHP() { return maxHp; }
	void setHP(int _hp) { hp = _hp; }
	void setMaxHP(int _hp) { maxHp = _hp; }

	void setPlayerTrans(osg::ref_ptr<osg::MatrixTransform> _t) { playerTransform = _t; }
	void setBridgeTrans(osg::ref_ptr<osg::MatrixTransform> _t) { bridgeTransform = _t; }
	void setHealthbarTrans(osg::ref_ptr<osg::MatrixTransform> _t) { healthbarTransform = _t; }
	void rotateGunnerTrans(osg::Quat _q);

	void resetPlayer();

	void reScale(float _scaleX, float _scaleY);

	void setName(std::string _n) { name = _n; }

	virtual ~Player() {}

	Player operator=(Player _g);

private:
		osg::ref_ptr<osg::MatrixTransform> playerTransform;
		osg::ref_ptr<osg::MatrixTransform> bridgeTransform;
		osg::ref_ptr<osg::MatrixTransform> gunnerTransform;
		osg::ref_ptr<osg::MatrixTransform> healthbarTransform;
		
		osg::Vec3f pos = osg::Vec3f(0,0,0);

		GameObject bridge;

		int hp;
		int maxHp;
		float rigidBodyRadius;
		float scale = 1.0;

		//The name variable is currently not used. Later it may be used for multiplayer games etc.
		std::string name;
};
