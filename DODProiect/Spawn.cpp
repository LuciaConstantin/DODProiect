#include "Spawn.h"


void Spawn::spawnEntity(int count, int screenW, int screenH, gameMode mode)
{
	srand((unsigned)time(NULL));
	entityState state;

	for (int i = 0; i < count; i++) {
		int randomX = rand() % (screenW - 30);
		int randomY = rand() % (screenH - 15);
	
		if (mode == NSEW) {
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
		else {
			int minVal = -2;
			int maxVal = 2;

			int xRand, yRand;

			do {
				xRand = rand() % (maxVal - minVal + 1) + minVal;
				yRand = rand() % (maxVal - minVal + 1) + minVal;
			} while (xRand == 0 && yRand == 0);
			
			obj.emplace_back(randomX, randomY, RANDOM_MOVEMENT, xRand, yRand);

		}
		
	}
}











void Spawn::drawAll(SDL_Renderer* renderer) {
	for (auto& e : obj)
		e.drawEntity(renderer);
}


std::vector<entity>& Spawn::getObjects() { 
	return obj; 
}

