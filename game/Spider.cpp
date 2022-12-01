#include "stdafx.h"
#include "Spider.h"

CFish::CFish(Sint16 x, Sint16 y, char* pFileBitmap, Uint32 time, string color) : CSprite(x, y, 0, 0, time)
{
	LoadImage(pFileBitmap,CColor::White());
	SetImage(pFileBitmap);
	if (color == "1") SetProperty("tag", "pink");
	if (color == "2") SetProperty("tag", "silver");
	if (color == "3") SetProperty("tag", "gold");

	wanderTarget = CVector(0, 1);
	health = 100;
	fullness = 0;
	state = NONE;
	ChangeState(FULL);
}

CFish::CFish(Sint16 x, Sint16 y, CGraphics* pGraphics, Uint32 time, string color) : CSprite(x, y, 0, 0, time)
{
	if (color == "1") SetProperty("tag", "pink");
	if (color == "2") SetProperty("tag", "silver");
	if (color == "3") SetProperty("tag", "gold");

	wanderTarget = CVector(0, 1);
	health = 100;
	fullness = 0;
	state = NONE;
	ChangeState(FULL);
}

CFish::~CFish(void)
{
}

// Behaviour Steerings

CVector CFish::seek(CVector targetPos, float speed)
{
	// TO DO [1]: Implement the SEEK behaviour. It is used by the fox
	CVector desiredVelocity, steering;

	desiredVelocity = targetPos - GetPos();
	desiredVelocity = Normalise(desiredVelocity) * speed;
	steering = desiredVelocity - GetVelocity();
	return steering;
}

CVector CFish::flee(CVector targetPos, float speed)
{
	// TO DO [2]: Implement the FLEE behaviour. It is used by the rabbit
	CVector desiredVelocity, steering;

	desiredVelocity = GetPos() - targetPos;
	desiredVelocity = Normalise(desiredVelocity) * speed;
	steering = desiredVelocity - GetVelocity();
	return steering;

}

float random()
{
	return ((float)rand() / (float)RAND_MAX);
}

CVector CFish::wander(float speed, float wanderDist, float wanderRadius)
{
	//target is "wanderTarget a 'static' variable previously defined"
	wanderTarget += CVector((random() - 0.5), (random() - 0.5));
	wanderTarget = Normalise(wanderTarget);

	CVector realTarget = GetPosition() + Normalise(GetVelocity()) * wanderDist + wanderTarget * wanderRadius;
	return seek(realTarget, speed);
}

/*
void CFish::Attacked()
{
	ChangeState(DIE);
}
*/

void CFish::OnUpdate(Uint32 time, Uint32 deltaTime)
{
	// State-dependent actions
	switch (state)
	{
	case IDLE:
		break;
	case HUNGRY:
		health = health - 0.1;
		break;
	case FULL:
		health = 100;
		fullness = fullness - 0.2;
		break;
	case DIE:
		break;
	}


	// if in motion, rotate so that to follow the direction of the motion; otherwise reset rotation
	if (GetSpeed() > 0)
		SetRotation(GetDirection() - 90);
	// Transitions
	float enemyDistance = Distance(enemyPosition, GetPosition());
	switch (state)
	{
	case IDLE:
		//Default, used for testing
		ChangeState(HUNGRY);
		break;
	case HUNGRY:
		if (health <= 0) ChangeState(DIE);
		break;
	case FULL:
		if (fullness <= 0) ChangeState(HUNGRY);
		break;
	case DIE:
		//Fish that loose all their health or are eaten will be given the DIE state
		break;
	}

	CSprite::OnUpdate(time, deltaTime);
}

void CFish::ChangeState(STATE newState)
{
	if (newState == state)
		return;		// No state change

	state = newState;

	switch (state)
	{
	case IDLE:
		//Default, used for testing
		break;
	case HUNGRY:
		break;
	case FULL:
		fullness = 50;
		break;
	case DIE:
		//Fish that loose all their health or are eaten will be given the DIE state
		break;
	}
}

void CFish::wrapToScreen(int width, int height)
{
	if (GetX() < 0) SetX((float)width);
	if (GetX() > width) SetX(0);
	if (GetY() < 0) SetY((float)height);
	if (GetY() > height) SetY(0);

}
