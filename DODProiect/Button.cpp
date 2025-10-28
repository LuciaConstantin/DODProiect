#include "Button.h"


void Button::clickButton(entity& ent)
{ 
	// the button is cliked
	
	if (buttonType == 'i') {
		ent.increaseSpeed();
	}
	else {
		ent.decreaseSpeed();
	}
	
}

void Button::drawButton(SDL_Renderer* renderer)
{

	SDL_FRect rect;
	rect.x = x;
	rect.y = y;
	rect.h = h;
	rect.w = w;

	SDL_SetRenderDrawColor(renderer, 159, 96, 144, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void Button::setButtonX(int x)
{
	this -> x = x;
}

void Button::setButtonY(int y)
{
	this -> y = y;
}

int Button::getButtonX()
{
	return x;
}

int Button::getButtonY()
{
	return y;
}

int Button::getButtonW()
{
	return w;
}

int Button::getButtonH()
{
	return h;
}

