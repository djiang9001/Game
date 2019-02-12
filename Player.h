#ifndef PLAYER_H
#define PLAYER_H

#include "Constants.h"
#include "Enemy.h"

//Box collision detector
bool checkCollision(SDL_Rect a, SDL_Rect b);

class Player
{
public:
	//The dimensions of the player
	static const int PLAYER_WIDTH = HUMAN_WIDTH;
	static const int PLAYER_HEIGHT = HUMAN_HEIGHT;

	//Maximum axis velocity of the player
	static const int PLAYER_VEL = 10;

	//Initializes the variables
	Player();

	//Takes key presses and adjusts the player's velocity
	void handleEvent(SDL_Event& e);

	//Moves the player and checks collision with floor
	void move(SDL_Rect& floor, std::vector<Enemy>* enemies);

	//Shows the player on the screen
	void render();

private:
	//The X and Y offsets of the player
	int mPosX, mPosY;

	//The velocity of the player
	int mVelX, mVelY;

	//Player's collision box
	SDL_Rect mCollider;

	//Player's hitbox (attack)
	SDL_Rect aCollider;
	bool attack;
	int attackCounter;
	bool dead;
};

#endif
