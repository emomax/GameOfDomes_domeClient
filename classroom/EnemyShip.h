#include "Includes.h"
#include "GameObject.h"
#include "Projectile.h"

class EnemyShip : public GameObject
{
public:
	EnemyShip();
	EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, int _hp, int _id);

	void updateAI(osg::Vec3f _playerPos, std::list<Projectile>& _missiles, osg::ref_ptr<osg::MatrixTransform> _mSceneTrans, float _dt);

	virtual ~EnemyShip() {}

private:
	float attackCooldown;
	float homingMissileAttackCooldown;

	

	enum currState;
};
