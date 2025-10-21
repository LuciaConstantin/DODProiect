#include "entity.h"



void entity::drawEntity(SDL_Renderer* renderer)
{
	SDL_FRect rect;
	rect.x = x; 
	rect.y = y; 
	rect.h = h;
	rect.w = w;
	
	SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
	//SDL_RenderRect(renderer, &rect);
	SDL_RenderFillRect(renderer, &rect);
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
