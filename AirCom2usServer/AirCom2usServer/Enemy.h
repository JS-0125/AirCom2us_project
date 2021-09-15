#pragma once
#include "default.h"
#include"Object.h"

using namespace chrono;

enum ENEMY_TYPE { ENEMY_Plane1, ENEMY_Plane2, ENEMY_Plane3, ENEMY_Boss1, ENEMY_Boss2 };

class Enemy: public Object
{
private:
public:
	virtual void CreateEnemy(float x, float y) {};
	virtual void Move( int& time) {};
};

class Plane1 : public Enemy {
public:
	 void CreateEnemy(float x, float y) override {}
	 void Move(int& time) override {
		 //cout << m_x << ", " << m_y;
		 //cout << "time: " << time << ", m_movetime: " << m_move_time << endl;;
		 auto delTime = time - m_move_time;
		 m_y -= delTime * 0.001;
		 m_move_time = time;
	 }
};

class Plane2 : public Plane1 {
public:
};

class Plane3 : public Enemy {
private:
	int hp;

public:
	void CreateEnemy(float x, float y) override {}
	void Move(int& time) override {
		auto delTime = time - m_move_time;
		m_x = (2 * sin(time*0.001));
		m_y -= delTime * 0.001;
		m_move_time = time;
	}
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