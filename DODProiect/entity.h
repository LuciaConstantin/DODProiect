#pragma once
#include <SDL3/SDL.h>
#include <cstdlib>
#include <ctime>

enum entityState { IDLE, MOVING_UP, MOVING_DOWN, MOVING_LEFT, MOVING_RIGHT };

class entity
{
private:
	int x, y;
	int w = 20, h= 20;
	entityState state;
	double step = 1;

public:
	entity() : x(0), y(0) {
	}
	entity(int x, int y, entityState state) {
		this->x = x;
		this->y = y;
		this->state = state;
	}

	entityState getState();
	int getX();
	int getY();
	int getStep();
	int getWidth();
	int getHeight();
	
	void setState(entityState state);
	void setX(int newX);
	void setY(int newY);

	void drawEntity(SDL_Renderer* renderer);
	void increaseSpeed();
	void decreaseSpeed();

};

