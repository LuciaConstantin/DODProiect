#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_stdinc.h>
#include "Spawn.h"
#include "Button.h"


namespace Colors {
	const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color GREEN = { 0, 255, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color WHITE = { 255, 255, 255, SDL_ALPHA_OPAQUE };
}

namespace Graphics {
	struct Screen {
		const int width = 1800;
		const int height = 900;
		const int centerX = width / 2;
		const int centerY = height / 2;
		const int messageSpace = 50;
	};
	struct smallScreen {
		const int width = 400;
		const int height = 300;
	};

	struct speedButtons {
		const int xDec = 50;
		const int yDec = 50;
		const int xInc = 150;
		const int yInc = 50;
		const int distanceBetween = xInc - xDec;
	};


}


struct App {
	const int DEFAULT_STEP = 2;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Window* subWindow = nullptr;
	SDL_Renderer* subRenderer = nullptr;

	Graphics::Screen screen;
	Graphics::smallScreen subScreen;
	Graphics::speedButtons buttons;
	gameMode game = RANDOM; // RANDOM NSEW

	Uint32 totalFrameTicks = 0;
	Uint32 totalFrames = 0;

} app;


bool InitSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cout << "SDL_Init failed with error: " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}


static void ClearScreen(SDL_Renderer* renderer, SDL_Renderer* subRenderer) {
	SDL_SetRenderDrawColor(renderer, Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, Colors::BLACK.a);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(subRenderer, Colors::WHITE.r, Colors::WHITE.g, Colors::WHITE.b, Colors::WHITE.a);
	SDL_RenderClear(subRenderer);


}


void ShutdownApplication() {
	if (app.window != nullptr) {
		SDL_DestroyWindow(app.window);
		app.window = nullptr;
	}

	if (app.renderer != nullptr) {
		SDL_DestroyRenderer(app.renderer);
		app.renderer = nullptr;
	}

	if (app.subWindow != nullptr) {
		SDL_DestroyWindow(app.subWindow);
		app.subWindow = nullptr;
	}

	if (app.subRenderer != nullptr) {
		SDL_DestroyRenderer(app.subRenderer);
		app.subRenderer = nullptr;
	}

	SDL_Quit();
}

bool InitApplication() {

	if (!InitSDL()) {
		ShutdownApplication();
		return false;
	}

	app.window = SDL_CreateWindow("Hello window", app.screen.width, app.screen.height, SDL_WINDOW_OPENGL);

	if (!app.window) {
		std::cout << "Unable to create the main window. Error: " << SDL_GetError() << std::endl;
		ShutdownApplication();
		return false;
	}

	app.renderer = SDL_CreateRenderer(app.window, nullptr);


	app.subWindow = SDL_CreateWindow("Hello small window", app.subScreen.width, app.subScreen.height, SDL_WINDOW_OPENGL);

	if (!app.subWindow) {
		std::cout << "Unable to create the small window. Error: " << SDL_GetError() << std::endl;
		ShutdownApplication();
		return false;
	}

	app.subRenderer = SDL_CreateRenderer(app.subWindow, nullptr);


	return true;
}


bool checkCollision(entity& a, entity& b) {
	return (a.getX() < b.getX() + b.getWidth()) && (a.getX() + a.getWidth() > b.getX()) &&
		(a.getY() < b.getY() + b.getHeight()) && (a.getY() + a.getHeight() > b.getY());
}


float checkOutOfScreenX(float x, int w) {
	if (x + w > app.screen.width) {
		x =(float) app.screen.width - w;
	}

	else if (x < 0) {
		x = 0.f;
	}

	return x;
}

float checkOutOfScreenY(float y, int h) {
	if (y + h > app.screen.height) {
		y =(float) app.screen.height - h;
	}
	else if (y < app.screen.messageSpace) {
		y = (float)app.screen.messageSpace;
	}
	return y;
}


void random(std::vector<entity>& obj) {
	
	if (app.game == RANDOM) {
		for (entity& en : obj) {
			int futureX = en.getX() + en.getxRand() * en.getStep();
			int futureY = en.getY() + en.getyRand() * en.getStep();

			if (futureX < 0) {
				en.setX(0);
				en.setxRand(-en.getxRand());
			}
			else if (futureX + en.getWidth() >= app.screen.width) {
				en.setX(app.screen.width - en.getWidth());
				en.setxRand(-en.getxRand());
			}

			if (futureY < app.screen.messageSpace) {
				en.setY(app.screen.messageSpace);
				en.setyRand(-en.getyRand());
			}
			else if (futureY + en.getHeight() >= app.screen.height) {
				en.setY(app.screen.height - en.getHeight());
				en.setyRand(-en.getyRand());
			}




		}
	}

	if (app.game == RANDOM) {
		for (entity& en : obj) {
			en.setX(en.getX() + en.getxRand() * en.getStep());
			en.setY(en.getY() + en.getyRand() * en.getStep());
		}

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
						obj[i].setX(checkOutOfScreenX(obj[i].getX() - overlapX / 2, obj[i].getWidth()));
						obj[j].setX(checkOutOfScreenX(obj[j].getX() + overlapX / 2, obj[j].getWidth()));
					}
					else {
						obj[i].setX(checkOutOfScreenX(obj[i].getX() + overlapX / 2, obj[i].getWidth()));
						obj[j].setX(checkOutOfScreenX(obj[j].getX() - overlapX / 2, obj[j].getWidth()));
					}
				}
				else {
					
					obj[i].setyRand(-obj[i].getyRand());
					obj[j].setyRand(-obj[j].getyRand());

					if (obj[i].getY() < obj[j].getY()) {
						obj[i].setY(checkOutOfScreenY(obj[i].getY() - overlapY / 2, obj[i].getHeight()));
						obj[j].setY(checkOutOfScreenY(obj[j].getY() + overlapY / 2, obj[j].getHeight()));
					}
					else {
						obj[i].setY(checkOutOfScreenY(obj[i].getY() + overlapY / 2, obj[i].getHeight()));
						obj[j].setY(checkOutOfScreenY(obj[j].getY() - overlapY / 2, obj[j].getHeight()));
					}
				}
			}
		}
	}




}


void NSEWMove(std::vector<entity>& obj) {
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

				obj[i].setX(clamp(obj[i].getX() - dx / 10, 0, app.screen.width - obj[i].getWidth()));
				obj[i].setY(clamp(obj[i].getY() - dy / 10, 0, app.screen.height - obj[i].getHeight()));
				obj[j].setX(clamp(obj[j].getX() + dx / 10, 0, app.screen.width - obj[j].getWidth()));
				obj[j].setY(clamp(obj[j].getY() + dy / 10, 0, app.screen.height - obj[j].getHeight()));
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
			if (new_y + (en.getHeight() / 2) < app.screen.height) {
				en.setY(en.getY() + en.getStep());
			}
			else {
				en.setY(new_y - (new_y + (en.getHeight() / 2) - app.screen.height));
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
			if (newTopLeftx + (en.getWidth() / 2) <= app.screen.width) {
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


int main(int argc, char* argv[]) {

	if (!InitApplication()) {
		ShutdownApplication();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	bool running = true;

	Spawn spawner;
	spawner.spawnEntity(1000, app.screen.width, app.screen.height, app.screen.messageSpace, app.game);

	Button buttonInc(app.buttons.xInc, app.buttons.yInc, 'i');
	Button buttonDec(app.buttons.xDec, app.buttons.yDec, 'd');


	Uint64 perCountFrequency = SDL_GetPerformanceFrequency();
	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double totalTimeMs = 0;
	unsigned int frameCount = 0;

	while (running) {


		ClearScreen(app.renderer, app.subRenderer);

		// draw buttons for entity speed
		buttonDec.drawButton(app.subRenderer);
		buttonInc.drawButton(app.subRenderer);

		SDL_RenderPresent(app.subRenderer);


		std::vector<entity>& obj = spawner.getObjects();

		buttonDec.drawButton(app.subRenderer);


		if (app.game == RANDOM) {
			random(obj);

		}
		else {
			NSEWMove(obj);

		}


		// keyboard events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_KEY_DOWN:
				if (event.key.scancode == SDL_SCANCODE_ESCAPE)
					running = false;
				break;

			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
				running = false;
				break;

			case SDL_EVENT_QUIT:
				running = false;
				break;
			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				/// check for increase/decrease speed in the small window
				if (event.button.windowID == SDL_GetWindowID(app.subWindow)) {
					// get mouse position
					float x = -1.f;
					float y = -1.f;
					SDL_GetMouseState(&x, &y);

					if (x >= buttonDec.getButtonX() && x <= buttonDec.getButtonX() + buttonDec.getButtonW()
						&& y >= buttonDec.getButtonY() && y <= buttonDec.getButtonY() + buttonDec.getButtonH()) {
						for (entity& en : obj) {
							buttonDec.clickButton(en);
						}

					}
					else if (x >= buttonInc.getButtonX() && x <= buttonInc.getButtonX() + buttonInc.getButtonW() &&
						y >= buttonInc.getButtonY() && y <= buttonInc.getButtonY() + buttonInc.getButtonH()) {
						for (entity& en : obj) {
							buttonInc.clickButton(en);
						}
					}
				}

				break;

			default:
				break;
			}
		}


		spawner.drawAll(app.renderer);



		// fps information
		Uint64 endCounter = SDL_GetPerformanceCounter();
		Uint64 counterElapsed = endCounter - lastCounter;

		double MSPerFrame = (((1000.0f * (double)counterElapsed) / (double)perCountFrequency));
		double FPS = (double)perCountFrequency / (double)counterElapsed;

		lastCounter = endCounter;

		totalTimeMs += MSPerFrame;
		frameCount++;

		double avgFPS = 1000.0 * (double)frameCount / (double)totalTimeMs;

		char* fps = NULL;
		char* avg = NULL;
		char* ms = NULL;

		SDL_asprintf(&fps, "Current FPS: %.2f", FPS);
		SDL_asprintf(&avg, "Average FPS: %.2f", avgFPS);
		SDL_asprintf(&ms, "Frame Time(ms): %.2f", MSPerFrame);



		SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
		SDL_RenderDebugText(app.renderer, 10, 10, fps);
		SDL_RenderDebugText(app.renderer, 10, 20, avg);
		SDL_RenderDebugText(app.renderer, 10, 30, ms);


		SDL_RenderPresent(app.renderer);


	}

	ShutdownApplication();

	return EXIT_SUCCESS;

}
