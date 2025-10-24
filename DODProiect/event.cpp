#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "Spawn.h"


namespace Colors {
	const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color GREEN = { 0, 255, 0, SDL_ALPHA_OPAQUE };

}

namespace Graphics {
	struct Screen {
		const int width = 1800;
		const int height = 900;
		const int centerX = width / 2;
		const int centerY = height / 2;
	};

}


struct App {
	const int DEFAULT_STEP = 2;
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Graphics::Screen screen;

	//Square square = { 72, 72, screen.centerX, screen.centerY, IDLE, DEFAULT_STEP };

} app;



bool InitSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL_Init failed with error: " << SDL_GetError() << std::endl;
		return false;
	}
	return true;
}


static void ClearScreen(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, Colors::BLACK.r, Colors::BLACK.g, Colors::BLACK.b, Colors::BLACK.a);
	SDL_RenderClear(renderer);
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

	return true;
}


bool checkCollision(entity& a, entity& b) {
	return (a.getX() < b.getX() + b.getWidth()) && (a.getX() + a.getWidth() > b.getX()) &&
		(a.getY() < b.getY() + b.getHeight()) && (a.getY() + a.getHeight() > b.getY());
}


int main(int argc, char* argv[]) {

	if (!InitApplication()) {
		ShutdownApplication();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	bool running = true;

	Spawn spawner;
	spawner.spawnEntity(20, app.screen.width, app.screen.height);

	while (running) {
		ClearScreen(app.renderer);

		std::vector<entity>& obj = spawner.getObjects();


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
					en.setY( new_y - (new_y + (en.getHeight() / 2) - app.screen.height));
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


		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_KEY_DOWN: {
				if (event.key.scancode == SDL_SCANCODE_ESCAPE)
					running = false;

				break;
			}
			case SDL_EVENT_QUIT: {
				running = false;
				break;
			}
			default:
				break;
			}
		}


		spawner.drawAll(app.renderer);

		SDL_RenderPresent(app.renderer);

	}

	ShutdownApplication();

	return EXIT_SUCCESS;

}