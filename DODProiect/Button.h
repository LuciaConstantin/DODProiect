#pragma once
#include "entity.h"


class Button
{
private:
	int x;
	int y;
	int w = 40, h = 40;
	// decrease = d, increase = i
	char buttonType;
public:

	Button(int x, int y, char buttonType) {
		this->x = x;
		this->y = y;
		this->buttonType = buttonType;
	}

	void clickButton(entity& ent);
	void drawButton(SDL_Renderer* renderer);

	void setButtonX(int x);
	void setButtonY(int y);
	int getButtonX();
	int getButtonY();
	int getButtonW();
	int getButtonH();
};

