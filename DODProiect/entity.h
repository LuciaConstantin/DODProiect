#pragma once
#include <SDL3/SDL.h>
#include <cstdlib>
#include <ctime>

enum entityState { IDLE, MOVING_UP, MOVING_DOWN, MOVING_LEFT, MOVING_RIGHT, RANDOM_MOVEMENT};
enum gameMode {NSEW, RANDOM};

class entity
{
private:
	int x, y;
	int w = 20, h= 20;
	entityState state;
	double step = 1;
	// for radom movement
	int xRand = 0, yRand = 0;


public:
	entity() : x(0), y(0){
	}

	entity(int x, int y, entityState state) {
		this->x = x;
		this->y = y;
		this->state = state;
	}

	entity(int x, int y, entityState state, int xRand, int yRand) {
		this -> x = x;
		this -> y = y;
		this -> state = state;
		this -> xRand = xRand;
		this -> yRand = yRand;

	}

	entityState getState();
	int getX();
	int getY();
	int getStep();
	int getWidth();
	int getHeight();
	int getxRand();
	int getyRand();
	
	void setState(entityState state);
	void setX(int newX);
	void setY(int newY);
	void setxRand(int newX);
	void setyRand(int newY);

	void drawEntity(SDL_Renderer* renderer);
	void increaseSpeed();
	void decreaseSpeed();

};

