#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "entity.h"

class Spawn
{
private:
	std::vector<entity> obj;

public:
	void spawnEntity(int count, int screenW, int screenH);
	void drawAll(SDL_Renderer* renderer);
	std::vector<entity>& getObjects();
};

