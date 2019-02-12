#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <iostream>

#include "Player.h"
#include "Constants.h"

Player::Player()
{
	//Initialize the offsets
	mPosX = 10;
	mPosY = 376;

	//Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	//Set collision box dimension
	mCollider.w = PLAYER_WIDTH;
	mCollider.h = PLAYER_HEIGHT;

	//Set hitbox dimensions
	aCollider.w = PLAYER_WIDTH * 2.5;
	aCollider.h = PLAYER_HEIGHT;
	attack = false;
	attackCounter = 0;
	dead = false;
}

void Player::handleEvent(SDL_Event& e)
{
	//If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY -= PLAYER_VEL; break;
		case SDLK_DOWN: mVelY += PLAYER_VEL; break;
		case SDLK_LEFT: mVelX -= PLAYER_VEL; break;
		case SDLK_RIGHT: mVelX += PLAYER_VEL; break;
		case SDLK_SPACE: attack = true; break;
		case SDLK_x: dead = true; break;
		case SDLK_c: dead = false; break;
		}
	}
	//If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		//Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: mVelY += PLAYER_VEL; break;
		case SDLK_DOWN: mVelY -= PLAYER_VEL; break;
		case SDLK_LEFT: mVelX += PLAYER_VEL; break;
		case SDLK_RIGHT: mVelX -= PLAYER_VEL; break;
		}
	}
}

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

//Moves the player
//Player does not move, checks for attack hitbox collision/enemy collision instead. Make move a bool that returns false if dead so we know to end to play the death animation and end the game
void Player::move(SDL_Rect& floor, std::vector<Enemy>* enemies)
{
	//Move the player left or right
	mPosX += mVelX;
	mCollider.x = mPosX;
	aCollider.x = mPosX;

	//If the player went too far to the left or right
	if ((mPosX < 0) || (mPosX + PLAYER_WIDTH > SCREEN_WIDTH) || checkCollision(mCollider, floor))
	{
		//Move back
		mPosX -= mVelX;
		mCollider.x = mPosX;
	}

	//Move the player up or down
	mPosY += mVelY;
	mCollider.y = mPosY;
	aCollider.y = mPosY;

	//If the player went too far up or down
	if ((mPosY < 0) || (mPosY + PLAYER_HEIGHT > SCREEN_HEIGHT) || checkCollision(mCollider, floor))
	{
		//Move back
		mPosY -= mVelY;
		mCollider.y = mPosY;
	}

	//std::cout << "eCollider: (" << enemies->at(0).eCollider.x << " " << enemies->at(0).eCollider.y << ")\n";

	for (int i = 0; i < enemies->size();i++) {

		if (checkCollision(mCollider, enemies->at(i).eCollider))
		{
			dead = true;
		}
		else if (attack && checkCollision(aCollider, enemies->at(i).hCollider) && !dead)
		{
			enemies->at(i).dead = true;
			//enemies->at(i) = enemies->at(enemies->size() - 1);
			//enemies->pop_back();
		}
		
	}

	if (dead)
		attack = false;
}
