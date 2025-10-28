#include "entity.h"



void entity::drawEntity(SDL_Renderer* renderer)
{
	SDL_FRect rect;
	rect.x = x; 
	rect.y = y; 
	rect.h = h;
	rect.w = w;
	
	SDL_SetRenderDrawColor(renderer, 36, 219, 193, 255);
	//SDL_RenderRect(renderer, &rect);
	SDL_RenderFillRect(renderer, &rect);
}

void entity::increaseSpeed()
{
	if (step + 1 < 10) {
		step += 1;
	}
	
}

void entity::decreaseSpeed()
{
	if (step - 1 > 0) {
		step -= 1;
	}
	else
		step = 1;
}

void entity::setState(entityState state)
{
	this->state = state;
}

void entity::setX(int newX)
{
	x = newX;
}

void entity::setY(int newY)
{
	y = newY;
}

void entity::setxRand(int newX)
{
	xRand = newX;
}

void entity::setyRand(int newY)
{
	yRand = newY;
}

entityState entity::getState()
{
	return state;
}

int entity::getX()
{
	return x;
}

int entity::getY()
{
	return y;
}

int entity::getStep()
{
	return step;
}

int entity::getWidth()
{
	return w;
}

int entity::getHeight()
{
	return h;
}

int entity::getxRand()
{
	return xRand;
}

int entity::getyRand()
{
	return yRand;
}
