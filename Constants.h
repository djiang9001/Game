#ifndef CONSTANTS_H
#define CONSTANTS_H

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int HUMAN_WIDTH = 32;
const int HUMAN_HEIGHT = 64;

const int ENEMY_SPEED_MIN = -2;
const int ENEMY_SPEED_MAX = -20;
const int ENEMY_SPEED_INCR = -2;

enum EnemyType
{
	SWORD_ATTACK,
	SWORD_BLOCK,
	KNIFE,
	KNIFE_THROW
};

#endif
