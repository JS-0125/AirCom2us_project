#pragma once
#include <chrono>
#include"Object.h"

using namespace std;
using namespace chrono;

enum ENEMY_TYPE { ENEMY_Plane1, ENEMY_Plane2, ENEMY_Plane3, ENEMY_Boss1, ENEMY_Boss2 };
enum ENEMY_STATE { ENEMY_FREE, ENEMY_INGAME };

class Enemy: public Object
{
public:
	ENEMY_STATE type = ENEMY_FREE;

	virtual void CreateEnemy(float x, float y) {};
};

class Plane1 : public Enemy {
public:
	 void CreateEnemy(float x, float y) override {}
	 void Move(float& x, float& y, float& time) {
		 y -= (duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - time) * 0.1;
	 }
};

class Plane2 : public Enemy {
public:
	void CreateEnemy(float x, float y) override {}
};

class Plane3 : public Enemy {
private:
	int hp;

public:
	void CreateEnemy(float x, float y) override {}
};

class Boss1 : public Enemy {
private:
	int hp;
public:
	void CreateEnemy(float x, float y) override {}
};

class Boss2 : public Enemy {
private:
	int hp;
public:
	void CreateEnemy(float x, float y) override {}
};