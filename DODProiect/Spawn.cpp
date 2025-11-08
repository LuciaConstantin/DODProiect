#include "Spawn.h"


void Spawn::spawnEntity(int count, int screenW, int screenH, int screenHStart, gameMode mode)
{
	srand((unsigned)time(NULL));
	entityState state;
	entity ent;

	for (int i = 0; i < count; i++) {
		float randomX = (float)(rand() % (screenW - ent.getWidth()));
		float randomY = (float)(screenHStart + rand() % (screenH - screenHStart - ent.getHeight()));
	
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

			float xRand, yRand;

			do {
				xRand =(float) (rand() % (maxVal - minVal + 1) + minVal);
				yRand =(float) (rand() % (maxVal - minVal + 1) + minVal);
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

