#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>



namespace Colors {
	const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color GREEN = { 0, 255, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color WHITE = { 255, 255, 255, SDL_ALPHA_OPAQUE };
	const SDL_Color MINT = { 143, 206, 150, SDL_ALPHA_OPAQUE };
}

// About the game world
namespace Graphics {
	struct Screen {
		const int width = 1900;
		const int height = 980;
		const int messageSpace = 50;
	};

	struct entityDimensions {
		const int w = 20;
		const int h = 20;
		const float step = 0.1;
	};

	struct gameData {
		const int nEntity = 1000; // number of enities on the screen
		// number of columns and rows for building the grid
		const int nCols = 50; 
		const int nRows = 50;
	};

}


struct App {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	Graphics::Screen screen;
	Graphics::entityDimensions dim;
	Graphics::gameData gameData;

	Uint64 totalFrameTicks = 0; // For fps

} app;


// Data structures for entity - grid

std::vector<std::vector<int>> grids(
	app.gameData.nCols* app.gameData.nRows,                   // number of cels
	std::vector<int>(app.gameData.nEntity / (app.gameData.nCols * app.gameData.nRows), 0) // all the entities that can be in a cell
);




bool InitSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
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

	app.renderer = SDL_CreateRenderer(app.window, "opengl");


	return true;
}

// assign values for the entities
void create(int N, float*& x, float*& y, int*& w, int*& h, float*& step, float*& xRand, float*& yRand) {
	srand((unsigned)time(NULL));

	for (int i = 0; i < N; i++) {
		w[i] = app.dim.w;
		h[i] = app.dim.h;
		step[i] = app.dim.step;

	}

	for (int i = 0; i < N; i++) {
		x[i] = (float)(rand() % (app.screen.width - w[i]));
		y[i] = (float)(app.screen.messageSpace + rand() % (app.screen.height - app.screen.messageSpace - h[i]));

		
		int minVal = -2;
		int maxVal = 2;

		// ex: rand() % (2 - (-2) + 1 ) + (-2) <=> rand() % 5 - 2  <=> 1 - 2 = -1
		do {
			xRand[i] = (float)(rand() % (maxVal - minVal + 1) + minVal);
			yRand[i] = (float)(rand() % (maxVal - minVal + 1) + minVal);
		} while (xRand[i] == 0 && yRand[i] == 0);

	}

}

// draw entities as rectangles
void drawEntities(int N, float* x, float* y, int* w, int* h, SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, Colors::MINT.r, Colors::MINT.g, Colors::MINT.b, Colors::MINT.a);
	for (int i = 0; i < N; i++) {
		SDL_FRect rect;
		rect.x = (float)x[i];
		rect.y = (float)y[i];
		rect.h = (float)h[i];
		rect.w = (float)w[i];

		//SDL_SetRenderDrawColor(renderer, Colors::MINT.r, Colors::MINT.g, Colors::MINT.b, Colors::MINT.a);
		SDL_RenderFillRect(renderer, &rect);

	}

}


// move on the 0x 0y axes
void moveEntity(int N, float*& x, float*& y, int* w, int* h, float* step, float* xRand, float* yRand) {

	for (int i = 0; i < N; i++) {
		x[i] = x[i] + xRand[i] * step[i];
		y[i] = y[i] + yRand[i] * step[i];
	}
}


// entities can't escape the window, if they collide with the window they bounce back in the opposite direction
void checkOutOfFrame(int N, float*& x, float*& y, int* w, int* h, float* step, float*& xRand, float*& yRand) {
	
	for (int i = 0; i < N; i++) {
		float futureX = x[i] + xRand[i] * step[i];
		float futureY = y[i] + yRand[i] * step[i];

		if (futureX < 0) {
			x[i] = 0.f;
			xRand[i] = -xRand[i];
		}
		else if (futureX + w[i] >= app.screen.width) {
			x[i] = (float)app.screen.width - w[i];
			xRand[i] = -xRand[i];
		}

		if (futureY < app.screen.messageSpace) {
			y[i] = (float)app.screen.messageSpace;
			yRand[i] = -yRand[i];

		}
		else if (futureY + h[i] >= app.screen.height) {
			y[i] = (float)app.screen.height - h[i];
			yRand[i] = -yRand[i];
		}

	}
}

// check if after the entities collision the entities are still in the window after the coordinate change
float checkOutOfScreenX(float x, int w) {
	if (x + w > app.screen.width) {
		x = (float)app.screen.width - w;
	}

	else if (x < 0) {
		x = 0.f;
	}

	return x;
}

float checkOutOfScreenY(float y, int h) {
	if (y + h > app.screen.height) {
		y = (float)app.screen.height - h;
	}
	else if (y < app.screen.messageSpace) {
		y = (float)app.screen.messageSpace;
	}
	return y;
}


// check if the entities collide
bool checkCollision(float x1, float y1, float x2, float y2, int w1, int h1, int w2, int h2) {
	return (x1 < x2 + w2) && (x1 + w1 > x2) && (y1 < y2 + h2) && (y1 + h1 > y2);
}

//  if 2 entities collide they move in the opposite direction and also bounce with half the distance of the entites overlap
void collision(int N, float*& x, float*& y, float* step, float*& xRand, float*& yRand, int* w, int* h) {

	for (int i = 0; i < N; i++) {
		for (int j = i + 1; j < N; j++) {
			if (checkCollision(x[i], y[i], x[j], y[j], w[i], h[i], w[j], h[j])) {

				float overlapX = 0.f;
				float overlapY = 0.f;

				if (x[i] < x[j]) {
					overlapX = x[i] + w[i] - x[j];
				}
				else {
					overlapX = x[j] + w[j] - x[i];
				}

				if (y[i] < y[j]) {
					overlapY = y[i] + h[i] - y[j];
				}
				else {
					overlapY = y[j] + h[j] - y[i];

				}

				if (overlapX < overlapY) {
					xRand[i] = -xRand[i];
					xRand[j] = -xRand[j];

					if (x[i] < x[j]) {
						x[i] = checkOutOfScreenX(x[i] - overlapX / 2, w[i]);
						x[j] = checkOutOfScreenX(x[j] + overlapX / 2, w[j]);
					}
					else {
						x[i] = checkOutOfScreenX(x[i] + overlapX / 2, w[i]);
						x[j] = checkOutOfScreenX(x[j] - overlapX / 2, w[j]);
					}
				}

				else {
					yRand[i] = -yRand[i];
					yRand[j] = -yRand[j];

					if (y[i] < y[j]) {
						y[i] = checkOutOfScreenY(y[i] - overlapY / 2, h[i]);
						y[j] = checkOutOfScreenY(y[j] + overlapY / 2, h[j]);
					}
					else {
						y[i] = checkOutOfScreenY(y[i] + overlapY / 2, h[i]);
						y[j] = checkOutOfScreenY(y[j] - overlapY / 2, h[j]);
					}
				}
			}

		}
	}
}

void entityPositionGrid(float x[], float y[], int nrEntities) {
	
	int width_cell = app.screen.width / app.gameData.nCols;
	int height_cell = (app.screen.height - app.screen.messageSpace) / app.gameData.nRows;
	
	for (int i = 0; i < app.gameData.nCols * app.gameData.nRows; i++)
		grids[i].clear();

	for (int i = 0; i < nrEntities; i++) {
		int qx = x[i] / width_cell;
		int qy = (y[i] - app.screen.messageSpace) / height_cell;

		qx = std::clamp(qx, 0, app.gameData.nCols - 1);
		qy = std::clamp(qy, 0, app.gameData.nRows - 1);


		int quad = qx + app.gameData.nCols * qy;
		grids[quad].push_back(i);
	}

	

}

void handleCollisions(int N, float*& x, float*& y, float* step, float*& xRand, float*& yRand, int* w, int* h) {
	entityPositionGrid(x, y, N);

	for (int k = 0; k < 50 * 50; k++) {
		for (int m = 0; m < grids[k].size(); m++) {
			int i = grids[k][m];
			
			for (int n = m + 1; n < grids[k].size(); n++) {	
				int j = grids[k][n];

				if (checkCollision(x[i], y[i], x[j], y[j], w[i], h[i], w[j], h[j])) {

					float overlapX = 0.f;
					float overlapY = 0.f;

					if (x[i] < x[j]) {
						overlapX = x[i] + w[i] - x[j];
					}
					else {
						overlapX = x[j] + w[j] - x[i];
					}

					if (y[i] < y[j]) {
						overlapY = y[i] + h[i] - y[j];
					}
					else {
						overlapY = y[j] + h[j] - y[i];

					}

					if (overlapX < overlapY) {
						xRand[i] = -xRand[i];
						xRand[j] = -xRand[j];

						if (x[i] < x[j]) {
							x[i] = checkOutOfScreenX(x[i] - overlapX / 2, w[i]);
							x[j] = checkOutOfScreenX(x[j] + overlapX / 2, w[j]);
						}
						else {
							x[i] = checkOutOfScreenX(x[i] + overlapX / 2, w[i]);
							x[j] = checkOutOfScreenX(x[j] - overlapX / 2, w[j]);
						}
					}

					else {
						yRand[i] = -yRand[i];
						yRand[j] = -yRand[j];

						if (y[i] < y[j]) {
							y[i] = checkOutOfScreenY(y[i] - overlapY / 2, h[i]);
							y[j] = checkOutOfScreenY(y[j] + overlapY / 2, h[j]);
						}
						else {
							y[i] = checkOutOfScreenY(y[i] + overlapY / 2, h[i]);
							y[j] = checkOutOfScreenY(y[j] - overlapY / 2, h[j]);
						}
					}
				}

			}

		}

	}

}




int main(int argc, char* argv[]) {

	float* x = new float[app.gameData.nEntity];
	float* y = new float[app.gameData.nEntity];
	float* step = new float[app.gameData.nEntity];
	float* xRand = new float[app.gameData.nEntity]; // for x axis movement 
	float* yRand = new float[app.gameData.nEntity]; // for y axis movement
	int* w = new int[app.gameData.nEntity];
	int* h = new int[app.gameData.nEntity];

	create(app.gameData.nEntity, x, y, w, h, step, xRand, yRand);



	if (!InitApplication()) {
		ShutdownApplication();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	bool running = true;

	Uint64 perCountFrequency = SDL_GetPerformanceFrequency();
	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double totalTimeMs = 0;
	unsigned int frameCount = 0;

	while (running) {
		ClearScreen(app.renderer);
 
		handleCollisions(app.gameData.nEntity, x, y, step, xRand, yRand, w, h);

		checkOutOfFrame(app.gameData.nEntity, x, y, w, h, step, xRand, yRand);
		moveEntity(app.gameData.nEntity, x, y, w, h, step, xRand, yRand);
		//collision(app.gameData.nEntity, x, y, step, xRand, yRand, w, h);
		

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

			default:
				break;
			}
		}

		drawEntities(app.gameData.nEntity, x, y, w, h, app.renderer);


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


	delete[] x;
	delete[] y;
	delete[] w;
	delete[] h;
	delete[] xRand;
	delete[] yRand;
	delete[] step;


	ShutdownApplication();

	return EXIT_SUCCESS;

}





