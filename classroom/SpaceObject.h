#include "GameObject.h"

class SpaceObject : public osg::Transform
{
public:
	SpaceObject();

	float getHP(){ return currHP; }
	float getMaxHP(){ return maxHP; }

	void setHP(float _h);

private:
	float currHP;
	float maxHP;
};
