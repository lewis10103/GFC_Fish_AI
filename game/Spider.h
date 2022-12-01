#pragma once

class CFish : public CSprite
{
	CVector wanderTarget;
public:
	enum STATE { NONE, IDLE, HUNGRY, FULL, DIE };
	float PATROL_SPEED = 20;
	float CHASE_SPEED = 30;

private:
	STATE state;
	CVector enemyPosition;
	float health;
	float fullness;

public:
	CFish(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time, string color);
	CFish(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time, string color);
	~CFish(void);

	void OnUpdate(Uint32 time, Uint32 deltaTime);

	void ChangeState(STATE newState);
	STATE GetState() { return state; }

	float GetHealth() { return health; }

	void SetEnemyPosition(CVector v) { enemyPosition = v; }
	void Attacked();

	// Behaviour Steerings
	CVector seek(CVector targetPos, float speed);
	CVector flee(CVector targetPos, float speed);
	CVector wander(float speed, float wanderDist = 1, float wanderRadius = 1);

	// Screen function
	void wrapToScreen(int width, int height);
};

typedef std::_gfc_std_ext<std::list<CFish*>> CFishList;
