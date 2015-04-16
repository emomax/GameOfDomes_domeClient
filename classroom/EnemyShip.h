#include "SpaceShip.h"

class EnemyShip : public osg::Transform
{
public:
	EnemyShip();
	
	void react();

private:
	std::string attackMode;
	enum currState;
};
