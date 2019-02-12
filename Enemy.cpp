#include "Constants.h"
#include "Enemy.h"


Enemy::Enemy()
{
	enemyHeight = HUMAN_HEIGHT;
	enemyWidth = HUMAN_WIDTH;
	ePosX = SCREEN_WIDTH;
	ePosY = 376;
	eVelX = ENEMY_SPEED_MIN;
	eCollider.y = ePosY;
	hCollider.y = ePosY;
	eCollider.h = HUMAN_HEIGHT;
	eCollider.w = HUMAN_WIDTH;
	hCollider.w = HUMAN_WIDTH;
	hCollider.h = HUMAN_HEIGHT;
	dead = false;

}

void Enemy::move()
{
	ePosX += eVelX;
	eCollider.x = ePosX;
	hCollider.x = ePosX;
}
