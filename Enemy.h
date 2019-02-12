#ifndef ENEMY_H
#define ENEMY_H

#include <SDL.h>

class Enemy
{
public:
	int enemyWidth;
	int enemyHeight;
	void move();
	void render();

	int enemyType;
	SDL_Rect eCollider;
	SDL_Rect hCollider;

	Enemy();

	int ePosX, ePosY;
	int eVelX;
	bool dead;
};

#endif