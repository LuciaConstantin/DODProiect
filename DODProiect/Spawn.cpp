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


float Spawn::checkOutOfScreenX(float x, int w, int W) {
	if (x + w > W) {
		x = (float)W - w;
	}

	else if (x < 0) {
		x = 0.f;
	}

	return x;
}

float Spawn::checkOutOfScreenY(float y, int h, int H, int ms) {
	if (y + h > H) {
		y = (float)H - h;
	}
	else if (y < ms) {
		y = (float)ms;
	}
	return y;
}

void Spawn::random(std::vector<entity>& obj, int w, int h, int s) {

	
		for (entity& en : obj) {
			int futureX = en.getX() + en.getxRand() * en.getStep();
			int futureY = en.getY() + en.getyRand() * en.getStep();

			if (futureX < 0) {
				en.setX(0);
				en.setxRand(-en.getxRand());
			}
			else if (futureX + en.getWidth() >= w) {
				en.setX(w - en.getWidth());
				en.setxRand(-en.getxRand());
			}

			if (futureY < s) {
				en.setY(s);
				en.setyRand(-en.getyRand());
			}
			else if (futureY + en.getHeight() >= h) {
				en.setY(h - en.getHeight());
				en.setyRand(-en.getyRand());
			}
		
	}

	
		for (entity& en : obj) {
			en.setX(en.getX() + en.getxRand() * en.getStep());
			en.setY(en.getY() + en.getyRand() * en.getStep());
		}

	


	for (size_t i = 0; i < obj.size(); ++i) {
		for (size_t j = i + 1; j < obj.size(); ++j) {
			if (checkCollision(obj[i], obj[j])) {


				float overlapX = 0.f;
				float overlapY = 0.f;


				if (obj[i].getX() < obj[j].getX())
					overlapX = obj[i].getX() + obj[i].getWidth() - obj[j].getX();
				else
					overlapX = obj[j].getX() + obj[j].getWidth() - obj[i].getX();


				if (obj[i].getY() < obj[j].getY())
					overlapY = obj[i].getY() + obj[i].getHeight() - obj[j].getY();
				else
					overlapY = obj[j].getY() + obj[j].getHeight() - obj[i].getY();


				if (overlapX < overlapY) {

					obj[i].setxRand(-obj[i].getxRand());
					obj[j].setxRand(-obj[j].getxRand());

					if (obj[i].getX() < obj[j].getX()) {
						obj[i].setX(checkOutOfScreenX(obj[i].getX() - overlapX / 2, obj[i].getWidth(), w));
						obj[j].setX(checkOutOfScreenX(obj[j].getX() + overlapX / 2, obj[j].getWidth(), w));
					}
					else {
						obj[i].setX(checkOutOfScreenX(obj[i].getX() + overlapX / 2, obj[i].getWidth(), w));
						obj[j].setX(checkOutOfScreenX(obj[j].getX() - overlapX / 2, obj[j].getWidth(), w));
					}
				}
				else {

					obj[i].setyRand(-obj[i].getyRand());
					obj[j].setyRand(-obj[j].getyRand());

					if (obj[i].getY() < obj[j].getY()) {
						obj[i].setY(checkOutOfScreenY(obj[i].getY() - overlapY / 2, obj[i].getHeight(), h, s));
						obj[j].setY(checkOutOfScreenY(obj[j].getY() + overlapY / 2, obj[j].getHeight(), h, s));
					}
					else {
						obj[i].setY(checkOutOfScreenY(obj[i].getY() + overlapY / 2, obj[i].getHeight(), h, s));
						obj[j].setY(checkOutOfScreenY(obj[j].getY() - overlapY / 2, obj[j].getHeight(), h, s));
					}
				}
			}
		}
	}




}


bool Spawn::checkCollision(entity& a, entity& b) {
	return (a.getX() < b.getX() + b.getWidth()) && (a.getX() + a.getWidth() > b.getX()) &&
		(a.getY() < b.getY() + b.getHeight()) && (a.getY() + a.getHeight() > b.getY());
}

// option for entities to only move NSEW not random
void Spawn::NSEWMove(std::vector<entity>& obj, int w, int h, int s) {
	// check entity movement and collision

	for (int i = 0; i < obj.size(); ++i) {
		for (int j = i + 1; j < obj.size(); ++j) {
			if (checkCollision(obj[i], obj[j])) {
				int dx = obj[j].getX() - obj[i].getX();
				int dy = obj[j].getY() - obj[i].getY();

				// coliziune pe verticala sau orizontala
				if (abs(dx) > abs(dy)) {
					if (dx > 0) {
						obj[i].setState(MOVING_LEFT);
						obj[j].setState(MOVING_RIGHT);
					}
					else {
						obj[i].setState(MOVING_RIGHT);
						obj[j].setState(MOVING_LEFT);
					}
				}
				else {
					if (dy > 0) {
						obj[i].setState(MOVING_UP);
						obj[j].setState(MOVING_DOWN);
					}
					else {
						obj[i].setState(MOVING_DOWN);
						obj[j].setState(MOVING_UP);
					}
				}

				auto clamp = [](int value, int min, int max) {
					return (value < min) ? min : (value > max ? max : value);
					};

				obj[i].setX(clamp(obj[i].getX() - dx / 10, 0, w - obj[i].getWidth()));
				obj[i].setY(clamp(obj[i].getY() - dy / 10, 0, h - obj[i].getHeight()));
				obj[j].setX(clamp(obj[j].getX() + dx / 10, 0, w - obj[j].getWidth()));
				obj[j].setY(clamp(obj[j].getY() + dy / 10, 0, h - obj[j].getHeight()));
			}
		}
	}



	// entity movement and escaping from the window

	for (entity& en : obj) {

		switch (en.getState()) {
		case MOVING_UP:
		{

			if ((en.getY() - en.getStep()) > (en.getHeight() / 2)) {
				en.setY(en.getY() - en.getStep());
			}
			else {
				en.setY(en.getHeight() / 2);
				en.setState(MOVING_DOWN);
			}
			break;
		}
		case MOVING_DOWN:
		{
			int new_y = en.getY() + en.getStep();
			if (new_y + (en.getHeight() / 2) < h) {
				en.setY(en.getY() + en.getStep());
			}
			else {
				en.setY(new_y - (new_y + (en.getHeight() / 2) - h));
				en.setState(MOVING_UP);

			}

			break;
		}
		case MOVING_LEFT:
		{
			int newTopLeftx = en.getX() - en.getStep() - (en.getWidth() / 2);
			if (newTopLeftx > en.getStep()) {
				en.setX(en.getX() - en.getStep());
			}
			else {
				en.setX(en.getWidth() / 2);
				en.setState(MOVING_RIGHT);

			}

			break;
		}
		case MOVING_RIGHT:
		{
			int newTopLeftx = en.getX() + en.getStep();
			if (newTopLeftx + (en.getWidth() / 2) <= w) {
				en.setX(en.getX() + en.getStep());
			}
			else {
				en.setState(MOVING_LEFT);
			}

			break;
		}
		default:
			break;
		}
	}


}