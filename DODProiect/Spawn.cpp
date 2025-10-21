#include "Spawn.h"


void Spawn::spawnEntity(int count, int screenW, int screenH)
{
	srand((unsigned)time(NULL));
	entityState state;

	for (int i = 0; i < count; i++) {
		int randomX = rand() % (screenW - 30);
		int randomY = rand() % (screenH - 15);
	
		switch (rand() % 5) {
		case 1: {
			state = MOVING_UP;
			break;
		}
		case 2: {
			state = MOVING_DOWN;
			break;
		}
		case 3: {
			state = MOVING_LEFT;
			break;
		}
		case 4: {
			state = MOVING_RIGHT;
			break;
		}
		default: {
			state = IDLE;
			break;
		}
		}
		obj.emplace_back(randomX, randomY, state);
	}
}


void Spawn::drawAll(SDL_Renderer* renderer) {
	for (auto& e : obj)
		e.drawEntity(renderer);
}


std::vector<entity>& Spawn::getObjects() { 
	return obj; 
}

