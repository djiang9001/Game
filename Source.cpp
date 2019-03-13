#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

//Constants for screen size, player size, enemy type enum
#include "Constants.h"

//My header files
#include "Player.h"
#include "Enemy.h"
#include "Timer.h"
#include "Textures.h"

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

LTexture gFPSTextTexture;
LTexture gStartTexture;

//Scene textures
LTexture gPlayerTexture;
LTexture gPlayerDeadTexture;
LTexture gEnemyTexture;
LTexture gEnemyDeadTexture;
LTexture gBGTexture;

LTexture gAttackTexture;

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load player texture
	if (!gPlayerTexture.loadFromFile("res/dot.bmp"))
	{
		printf("Failed to load player texture!\n");
		success = false;
	}

	if (!gPlayerDeadTexture.loadFromFile("res/dead.bmp"))
	{
		printf("Failed to load dead player texture!\n");
		success = false;
	}

	if (!gEnemyTexture.loadFromFile("res/enemy.bmp"))
	{
		printf("Failed to load enemy texture!\n");
		success = false;
	}

	if (!gEnemyDeadTexture.loadFromFile("res/enemydead.bmp"))
	{
		printf("Failed to load dead enemy texture!\n");
		success = false;
	}

	//Load background texture
	if (!gBGTexture.loadFromFile("res/bg.png"))
	{
		printf("Failed to load background texture!\n");
		success = false;
	}

	//Load attack texture
	if (!gAttackTexture.loadFromFile("res/attack.bmp"))
	{
		printf("Faile to load attack texture!\n");
		success = false;
	}

	//Open the font
	gFont = TTF_OpenFont("res/lazy.ttf", 28);
	if (gFont == NULL)
	{
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 0, 0, 0 };
		if (!gStartTexture.loadFromRenderedText("PRESS ENTER TO START", textColor))
		{
			printf("Failed to render text texture!\n");
			success = false;
		}
	}

	return success;
}

//End the program

void close()
{
	//Free loaded images
	gPlayerTexture.free();
	gPlayerDeadTexture.free();
	gEnemyTexture.free();
	gEnemyDeadTexture.free();
	gBGTexture.free();
	gAttackTexture.free();
	gFPSTextTexture.free();

	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("Game Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

void Enemy::render()
{
	if (dead)
	{
		gEnemyDeadTexture.render(ePosX, ePosY);
	}
	else if (ePosX > SCREEN_WIDTH/2)
	{
		gEnemyTexture.render(ePosX, ePosY);
	} //else if enemy is in range, render that enemy's attack animation, kill the player in player.move()
	else if (ePosX <= SCREEN_WIDTH / 2)
	{
		//replace this with a "ready" frame that reveals the type of enemy/preps for attack animation
		gEnemyTexture.render(ePosX, ePosY);
	}
}

//Renders the player

void Player::render()
{
	//Show the player
	gPlayerTexture.render(mPosX, mPosY);
	if (dead)
	{
		gPlayerDeadTexture.render(mPosX, mPosY);
	}
	else if (attack)
	{
		gAttackTexture.render(mPosX, mPosY);
		if (attackCounter > 4)
		{
			attack = false;
			attackCounter = 0;
		}
		else
			attackCounter++;
	}
}


int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Scene control flags
			bool play = false;
			bool paused = false;

			//Event handler
			SDL_Event e;

			//Game loop and initial variables for game loop
			//The application timer
			LTimer fpsTimer;
			fpsTimer.start();


			//These things need to be reinitialized for new games, but not for resuming paused games
			//game timer that keeps track of time for speed, spawning enemies
			LTimer timer;
			//start the game timer that will control enemy spawns and speed
			timer.start();

			// initialize variables, should make an FPSCalc class and a GameProgression class to manage and initialize these with a single function
			const int FPS_TOTAL = 100;
			int countedFrames = 1;
			int frameIndex = 0;
			int timeFrameArray[FPS_TOTAL] = {};
			double avgFPS = 0;
			std::stringstream fpsText;

			//Set text color as black
			SDL_Color textColor = { 0, 0, 0, 255 };

			//The player that will be moving around on the screen
			Player player;

			//Initialize values for different enemy types
			Enemy swordAttack;
			swordAttack.enemyType = SWORD_ATTACK;
			swordAttack.eCollider.w = HUMAN_WIDTH + 20;

			Enemy swordBlock;
			swordAttack.enemyType = SWORD_BLOCK;
			swordAttack.eCollider.w = HUMAN_WIDTH;

			Enemy knife;
			knife.enemyType = KNIFE;
			knife.eCollider.w = HUMAN_WIDTH + 10;

			Enemy knifeThrow;
			knifeThrow.enemyType = KNIFE_THROW;
			knifeThrow.eCollider.w = HUMAN_WIDTH + 10;

			//Vector of enemies
			std::vector<Enemy> enemies;

			//Variables that need to be set at start of game
			int timeSinceSpeedup;
			int timeSinceSpawn;
			int spawnProbability;

			//Set the floor
			SDL_Rect floor;
			floor.x = 0;
			floor.y = 440;
			floor.w = 640;
			floor.h = 40;

			//The background scrolling offset
			int scrollingOffset = 0;

			//While application is running
			while (!quit) {

				//while application is running but not playing
				while (!play && !quit) {
					SDL_RenderClear(gRenderer);
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					gStartTexture.render((SCREEN_WIDTH - gStartTexture.getWidth()) / 2, SCREEN_HEIGHT / 2);
					SDL_RenderPresent(gRenderer);
					while (SDL_PollEvent(&e) != 0)
					{
						//User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}

						if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
						{
							switch (e.key.keysym.sym)
							{
							case SDLK_RETURN:
								play = true;
								// reset starting variables (consider putting this in a function)
								enemies.clear();
								enemies.push_back(knife);

								timer.start();

								timeSinceSpeedup = 0;
								timeSinceSpawn = 0;
								spawnProbability = 20;

								break;
							}
						}

						//Handle input for the player
						player.handleEvent(e);
					}

					//render main menu elemnts

				}

				
				//While playing the game
				while (play && !quit && !paused) {

					//Handle events on queue
					while (SDL_PollEvent(&e) != 0)
					{
						//User requests quit
						if (e.type == SDL_QUIT)
						{
							quit = true;
						}

						if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
						{
							switch (e.key.keysym.sym)
							{
							case SDLK_p: 
								timer.pause();
								paused = true;
								break;
							case SDLK_RETURN: play = false; break;
							}
						}

						//Handle input for the player
						std::cout << "handling event" << std::endl;
						player.handleEvent(e);
					}
					//hold the game paused while it is paused
					//Still needs: to keep track of how long game has been paused so things like speeding up the game and fps timers don't get messed up from pausing
					
					while (paused) {

						//render pause menu, currently blank
						SDL_RenderClear(gRenderer);
						SDL_RenderPresent(gRenderer);

						while (SDL_PollEvent(&e) != 0)
						{
							//User requests quit
							if (e.type == SDL_QUIT)
							{
								quit = true;
							}


							//unpauses the game
							if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
							{
								switch (e.key.keysym.sym)
								{
								case SDLK_p: timer.unpause(); paused = false; break; //also rerender the previous textures onto screen so that the game doesnt play one frame unrendered
								case SDLK_RETURN: play = false; paused = false; break;
								}
							}

							//Handle input for the player
							player.handleEvent(e);
						}
					}

					// this is needed after the pause loop to prevent the game from playing one more frame before leaving to menu
					if (!play)
						break;

					//fps counting
					if (frameIndex == 100) {
						frameIndex = 0;
					}

					int currentTime = timer.getTicks();
					int fpsCurrentTime = fpsTimer.getTicks();

					timeFrameArray[frameIndex] = fpsCurrentTime;

					//Calculate and correct fps
					if (countedFrames == FPS_TOTAL) {
						avgFPS = (FPS_TOTAL - 1) / ((timeFrameArray[frameIndex] - timeFrameArray[(frameIndex + 1) % FPS_TOTAL]) / 1000.f);
					}
					else
					{
						avgFPS = (countedFrames - 1) / ((timeFrameArray[frameIndex] - timeFrameArray[0]) / 1000.);
						countedFrames++;
					}

					std::cout << currentTime << " fps: " << avgFPS << " speed: " << swordAttack.eVelX << std::endl;
					fpsText.str("");
					fpsText << "FPS: " << std::round(avgFPS);

					frameIndex++;

					//Render text
					if (!gFPSTextTexture.loadFromRenderedText(fpsText.str().c_str(), textColor))
					{
						printf("Unable to render FPS texture!\n");
					}

					//Speed up the game
					if (currentTime - timeSinceSpeedup > 5000 && swordAttack.eVelX > ENEMY_SPEED_MAX)
					{
						swordAttack.eVelX += ENEMY_SPEED_INCR;
						swordBlock.eVelX += ENEMY_SPEED_INCR;
						knife.eVelX += ENEMY_SPEED_INCR;
						knifeThrow.eVelX += ENEMY_SPEED_INCR;
						for (int i = 0; i < enemies.size(); i++)
						{
							enemies[i].eVelX += ENEMY_SPEED_INCR;
						}

						if (spawnProbability > 4) {
							spawnProbability--;
						}

						timeSinceSpeedup = currentTime;
					}

					//Spawn new enemies

					int genEnemy = rand() % spawnProbability;
					if (currentTime - timeSinceSpawn > 1000) {
						timeSinceSpawn = currentTime;
						if (genEnemy == 0) {
							int genEnemyType = rand() % 4;
							std::cout << "genEnemy: " << genEnemy << "genEnemyType: " << genEnemyType << std::endl;
							if (genEnemyType == SWORD_ATTACK)
							{
								enemies.push_back(swordAttack);
							}
							else if (genEnemyType == SWORD_BLOCK)
							{
								enemies.push_back(swordBlock);
							}
							else if (genEnemyType == KNIFE)
							{
								enemies.push_back(knife);
							}
							else if (genEnemyType == KNIFE_THROW)
							{
								enemies.push_back(knifeThrow);
							}
						}
					}

					//pointer of vector of enemies that can be passed to other functions
					std::vector<Enemy>* enemiesP = &enemies;

					//Move the player
					player.move(floor, enemiesP);

					//Move enemies
					for (int i = 0; i < enemies.size(); i++)
					{
						enemies[i].move();
						if (enemies[i].ePosX + enemies[i].enemyWidth < 0) {
							enemies[i] = enemies[enemies.size() - 1];
							enemies.pop_back();
						}
					}

					//Scroll background
					--scrollingOffset;
					if (scrollingOffset < -gBGTexture.getWidth())
					{
						scrollingOffset = 0;
					}

					//Clear screen
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//Render background
					gBGTexture.render(scrollingOffset, 0);
					gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);

					//Render floor
					SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
					SDL_RenderFillRect(gRenderer, &floor);

					//Render objects

					for (int i = 0; i < enemies.size(); i++)
					{
						enemies[i].render();
					}
					player.render();

					gFPSTextTexture.render(SCREEN_WIDTH - gFPSTextTexture.getWidth(), 0);


					//Update screen
					SDL_RenderPresent(gRenderer);

				}
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}