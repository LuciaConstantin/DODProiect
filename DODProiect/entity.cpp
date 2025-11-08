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
		step += 1.f;
	}
	
}

void entity::decreaseSpeed()
{
	if (step - 1 > 0) {
		step -= 1.f;
	}
	else
		step = 1.f;
}

void entity::setState(entityState state)
{
	this->state = state;
}

void entity::setX(float newX)
{
	x = newX;
}

void entity::setY(float newY)
{
	y = newY;
}

void entity::setxRand(float newX)
{
	xRand = newX;
}

void entity::setyRand(float newY)
{
	yRand = newY;
}

entityState entity::getState()
{
	return state;
}

float entity::getX()
{
	return x;
}

float entity::getY()
{
	return y;
}

float entity::getStep()
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

float entity::getxRand()
{
	return xRand;
}

float entity::getyRand()
{
	return yRand;
}
