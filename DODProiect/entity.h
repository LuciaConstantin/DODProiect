#pragma once
#include <SDL3/SDL.h>
#include <cstdlib>
#include <ctime>

enum entityState { IDLE, MOVING_UP, MOVING_DOWN, MOVING_LEFT, MOVING_RIGHT, RANDOM_MOVEMENT};
enum gameMode {NSEW, RANDOM};

class entity
{
private:
	float x, y;
	int w = 20, h= 20;
	entityState state;
	float step = 0.5f;
	// for radom movement
	float xRand = 0, yRand = 0;


public:
	entity() : x(0), y(0), state(IDLE){
	}

	entity(float x, float y, entityState state) {
		this->x = x;
		this->y = y;
		this->state = state;
	}

	entity(float x, float y, entityState state, float xRand, float yRand) {
		this -> x = x;
		this -> y = y;
		this -> state = state;
		this -> xRand = xRand;
		this -> yRand = yRand;

	}

	entityState getState();
	float getX();
	float getY();
	float getStep();
	int getWidth();
	int getHeight();
	float getxRand();
	float getyRand();
	
	void setState(entityState state);
	void setX(float newX);
	void setY(float newY);
	void setxRand(float newX);
	void setyRand(float newY);

	void drawEntity(SDL_Renderer* renderer);
	void increaseSpeed();
	void decreaseSpeed();

};

