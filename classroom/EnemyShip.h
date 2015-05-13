#include "GameObject.h"
#include "Includes.h"

class EnemyShip : public GameObject
{
public:
	EnemyShip();
	EnemyShip(std::string _name, osg::Vec3f _pos, float _colRad, std::string _model, osg::ref_ptr<osg::MatrixTransform> _scene, float _hp, int _id);

	int getHP() { return hp; }
	void setHp(int _hp) { hp = _hp; }

	void updateAI(osg::Vec3f _playerPos);

	virtual ~EnemyShip() {}

private:
	int hp;

	enum currState;
};
