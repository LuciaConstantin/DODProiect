#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "entity.h"

// class used to store all the entities and the intercations between them
class Spawn
{
private:
	std::vector<entity> obj;

public:
	void spawnEntity(int count, int screenW, int screenH, int screenHStart, gameMode mode);

	void drawAll(SDL_Renderer* renderer);
	
	std::vector<entity>& getObjects();
	
	float checkOutOfScreenX(float x, int w, int W);
	
	float checkOutOfScreenY(float y, int h, int H, int ms);

	void NSEWMove(std::vector<entity>& obj, int w, int h, int s);

	bool checkCollision(entity& a, entity& b);

	void random(std::vector<entity>& obj, int w, int h, int s);
};

