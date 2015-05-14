#include "Includes.h"
#include "GameObject.h"
#include "Projectile.h"

class EnemyShip : public GameObject
{
public:
	EnemyShip();
	EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _hp, int _id);

	int getHP() { return hp; }
	void setHp(int _hp) { hp = _hp; }

	void updateAI(osg::Vec3f _playerPos, std::list<Projectile>& _missiles, osg::ref_ptr<osg::MatrixTransform> _mSceneTrans, float _dt);

	virtual ~EnemyShip() {}

private:
	int hp;
	float attackCooldown;
	float homingMissileAttackCooldown;

	//Used for rotational purposes
	osg::Vec3f upDir;

	enum currState;
};
