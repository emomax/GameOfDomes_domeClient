#include "SpaceObject.h"

void SpaceObject::setHP(float _h)
{
	if (_h < maxHP)
		currHP = _h;
}
