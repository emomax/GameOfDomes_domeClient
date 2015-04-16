#include "SpaceObject.h"

class SpaceShip : public osg::Transform
{
public:
	SpaceShip();

	void fire();

private:
	float fireRate;
	float weaponDmg;
};
