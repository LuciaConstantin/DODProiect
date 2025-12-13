#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <omp.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <stdio.h>


#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

namespace Colors {
	const SDL_Color BLACK = { 0, 0, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color GREEN = { 0, 255, 0, SDL_ALPHA_OPAQUE };
	const SDL_Color WHITE = { 189, 160, 177, SDL_ALPHA_OPAQUE };
	const SDL_Color MINT = { 143, 206, 150, SDL_ALPHA_OPAQUE };
	const SDL_Color PURPLE = { 204, 108, 172 , SDL_ALPHA_OPAQUE };
}

// About the game world
namespace Graphics {
	struct Screen {
		const int width = 1536;
		const int height = 864;
		const int messageSpace = 50;
	};

	struct entityDimensions {
		const int w = 8;
		const int h = 8;
	};

	struct gameData {
		const int maxEntities = 100000; // number of enities on the screen
		int entityNumber = 10000;
		// number of columns and rows for building the grid
		const int nCols = 128; // 64 
		const int nRows = 72; // 36
		const int gridDistance = 12;
		const float step = 0.4; // entity speed
		
	};

}


struct App {

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	SDL_Window* menuWindow = nullptr;
	SDL_Renderer* menuRenderer = nullptr;

	float main_scale = 0.f;

	Graphics::Screen screen;
	Graphics::entityDimensions dim;
	Graphics::gameData gameData;

	Uint64 totalFrameTicks = 0; // For fps

} app;


// Data structures for entity - grid

std::vector<std::vector<int>> grids(
	app.gameData.nCols* app.gameData.nRows,            // number of cels
	std::vector<int>(app.gameData.maxEntities / (app.gameData.nCols * app.gameData.nRows), 0) // all the entities that can be in a cell
);




bool InitSDL() {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)){
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

bool InitApplicationMenu() {
	app.main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
	SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
	app.menuWindow = SDL_CreateWindow("Game menu", (int)(500 * app.main_scale), (int)(250 * app.main_scale), window_flags);
	if (app.menuWindow == nullptr)
	{
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return false;
	}
	app.menuRenderer = SDL_CreateRenderer(app.menuWindow, nullptr);
	SDL_SetRenderVSync(app.menuRenderer, 1);
	
	if (app.menuRenderer == nullptr)
	{
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return false;
	}
	SDL_SetWindowPosition(app.menuWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(app.menuWindow);

	return true;
}

// assign values for the entities
void create(int N, float*& x, float*& y, int*& w, int*& h, float*& dirX, float*& dirY) {
	
	srand((unsigned)time(NULL));

	for (int i = 0; i < N; i++) {
		w[i] = app.dim.w;
		h[i] = app.dim.h;
	}

	for (int i = 0; i < N; i++) {
		x[i] = (float)(rand() % (app.screen.width - w[i]));
		y[i] = (float)(app.screen.messageSpace + rand() % (app.screen.height - app.screen.messageSpace - h[i]));

		
		int minVal = -2;
		int maxVal = 2;

		// ex: rand() % (2 - (-2) + 1 ) + (-2) <=> rand() % 5 - 2  <=> 1 - 2 = -1
		do {
			dirX[i] = (float)(rand() % (maxVal - minVal + 1) + minVal);
			dirY[i] = (float)(rand() % (maxVal - minVal + 1) + minVal);
		} while (dirX[i] == 0 && dirY[i] == 0);

	}

}

// draw entities as rectangles
void drawEntities(int N, float* x, float* y, int* w, int* h, SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, Colors::PURPLE.r, Colors::PURPLE.g, Colors::PURPLE.b, Colors::PURPLE.a);
	for (int i = 0; i < N; i++) {
		SDL_FRect rect;
		rect.x = (float)x[i];
		rect.y = (float)y[i];
		rect.h = (float)h[i];
		rect.w = (float)w[i];

		SDL_RenderFillRect(renderer, &rect);

	}

}

void drawGrid(SDL_Renderer* renderer) {

	SDL_SetRenderDrawColor(renderer, Colors::WHITE.r, Colors::WHITE.g, Colors::WHITE.b, Colors::WHITE.a);
	for (int i = 0; i < app.gameData.nCols; i++) {
		SDL_RenderLine(renderer, i * app.gameData.gridDistance, app.screen.messageSpace, i * app.gameData.gridDistance, app.screen.height);
	}

	for (int i = 0; i < app.gameData.nRows; i++) {
		SDL_RenderLine(renderer, 0, i * app.gameData.gridDistance + app.screen.messageSpace , app.screen.width, i * app.gameData.gridDistance + app.screen.messageSpace);
	}
}


// move on the 0x 0y axes
void moveEntity(int N, float*& x, float*& y, int* w, int* h, float* dirX, float* dirY) {

	for (int i = 0; i < N; i++) {
		x[i] = x[i] + dirX[i] * app.gameData.step;
		y[i] = y[i] + dirY[i] * app.gameData.step;
	}
}



// entities can't escape the window, if they collide with the window they bounce back in the opposite direction

void checkOutOfFrame(int N, float*& x, float*& y, int* w, int* h, float*& velX, float*& velY) {
	
	for (int i = 0; i < N; i++) {
		float futureX = x[i] + velX[i] * app.gameData.step;
		float futureY = y[i] + velY[i] * app.gameData.step;

		if (futureX < 0) {
			x[i] = 0.f;
			velX[i] = -velX[i];
		}
		else if (futureX + w[i] >= app.screen.width) {
			x[i] = (float)app.screen.width - w[i];
			velX [i] = - velX[i];
		}

		if (futureY < app.screen.messageSpace) {
			y[i] = (float)app.screen.messageSpace;
			velY[i] = -velY[i];

		}
		else if (futureY + h[i] >= app.screen.height) {
			y[i] = (float)app.screen.height - h[i];
			velY[i] = -velY[i];
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
void collision(int N, float*& x, float*& y, float*& velX, float*& velY, int* w, int* h) {

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
					velX[i] = -velX[i];
					velX[j] = -velX[j];

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
					velY[i] = -velY[i];
					velY[j] = -velY[j];

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
	
	int width_cell =(int) app.screen.width / app.gameData.nCols;
	int height_cell =(int) (app.screen.height - app.screen.messageSpace) / app.gameData.nRows;
	
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


// handle collison between enitites where the entity is verified to collide only in it's grid
void handleCollisionsOrig(int N, float*& x, float*& y, float*& velX, float*& velY, int* w, int* h) {
	entityPositionGrid(x, y, N);
	
	#pragma omp parallel for
	for (int k = 0; k < app.gameData.nCols * app.gameData.nRows; k++) {
		for (int m = 0; m < grids[k].size(); m++) {
			int i = grids[k][m]; // index of an entity
			
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
						velX[i] = -velX[i];
						velX[j] = -velX[j];

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
						velY[i] = -velY[i];
						velY[j] = -velY[j];

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

void modifyAfterCollision(float& x1, float& x2, float& y1, float& y2, float& dirX1, float& dirY1, float& dirX2, float& dirY2, int w1, int w2,int h1, int h2) {

	float overlapX = 0.f;
	float overlapY = 0.f;

	if (x1 < x2) {
		overlapX = x1 + w1 - x2;
	}
	else {
		overlapX = x2 + w2 - x1;
	}

	if (y1 < y2) {
		overlapY = y1 + h1 - y2;
	}
	else {
		overlapY = y2 + h2 - y1;

	}

	if (overlapX < overlapY) {
		dirX1 = -dirX1;
		dirX2 = -dirX2;

		if (x1 < x2) {
			x1 = checkOutOfScreenX(x1 - overlapX / 2, w1);
			x2 = checkOutOfScreenX(x2 + overlapX / 2, w2);
		}
		else {
			x1 = checkOutOfScreenX(x1 + overlapX / 2, w1);
			x2 = checkOutOfScreenX(x2 - overlapX / 2, w2);
		}
	}

	else {
		dirY1 = -dirY1;
		dirY2 = -dirY2;

		if (y1 < y2) {
			y1 = checkOutOfScreenY(y1 - overlapY / 2, h1);
			y2 = checkOutOfScreenY(y2 + overlapY / 2, h2);
		}
		else {
			y1 = checkOutOfScreenY(y1 + overlapY / 2, h1);
			y2 = checkOutOfScreenY(y2 - overlapY / 2, h2);
		}
	}

}


// collison with neighbor grids
void handleCollisionsNeigh(int N, float*& x, float*& y, float*& dirX, float*& dirY, int* w, int* h) {
	entityPositionGrid(x, y, N);

	#pragma omp parallel for
	for (int k = 0; k < app.gameData.nCols * app.gameData.nRows; k++) {
		
		for (int m = 0; m < grids[k].size(); m++) {
			int i = grids[k][m]; // index of an entity

			for (int n = m + 1; n < grids[k].size(); n++) {
				int j = grids[k][n];

				if (checkCollision(x[i], y[i], x[j], y[j], w[i], h[i], w[j], h[j])) {
					modifyAfterCollision(x[i], x[j], y[i], y[j], dirX[i], dirY[i], dirX[j], dirY[j], w[i], w[j], h[i], h[j]);
				}	
			}

			int c1 = 4; 
			int c2 = 5; 
		
			if (k - c1 < 0) c1 = k;
			if (k + c2 >= app.gameData.nCols * app.gameData.nRows) c2 = app.gameData.nCols * app.gameData.nRows - 1 - k;

			
			for (int o = k - c1; o < k; o++) {
				for (int idx = 0; idx < grids[o].size(); idx++) {
					int id = grids[o][idx];

					if (checkCollision(x[i], y[i], x[id], y[id], w[i], h[i], w[id], h[id])) {
						modifyAfterCollision(x[i], x[id], y[i], y[id], dirX[i], dirY[i], dirX[id], dirY[id], w[i], w[id], h[i], h[id]);
					}
				}
			}

			for (int p = k + 1; p <= k + c2; p++) {
				for (int idx = 0; idx < grids[p].size(); idx++) {
					int id = grids[p][idx];

					if (checkCollision(x[i], y[i], x[id], y[id], w[i], h[i], w[id], h[id])) {
						modifyAfterCollision(x[i], x[id], y[i], y[id], dirX[i], dirY[i], dirX[id], dirY[id], w[i], w[id], h[i], h[id]);
					}
				}
			}

		}

	}

}




int main(int argc, char* argv[]) {

	float* x = new float[app.gameData.maxEntities];
	float* y = new float[app.gameData.maxEntities];
	float* dirX = new float[app.gameData.maxEntities]; // direction on the x axis
	float* dirY = new float[app.gameData.maxEntities]; // direction on the y axis
	int* w = new int[app.gameData.maxEntities];
	int* h = new int[app.gameData.maxEntities];
	
	
	create(app.gameData.maxEntities, x, y, w, h, dirX, dirY);

	if (!InitApplication()) {
		ShutdownApplication();
		return EXIT_FAILURE;
	}

	if (!InitApplicationMenu()) {
		ShutdownApplication();
		return EXIT_FAILURE;
	}

	SDL_Event event;
	bool running = true;

	Uint64 perCountFrequency = SDL_GetPerformanceFrequency();
	Uint64 lastCounter = SDL_GetPerformanceCounter();

	double totalTimeMs = 0;
	unsigned int frameCount = 0;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(app.main_scale);        
	style.FontScaleDpi = app.main_scale;  

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(app.menuWindow, app.menuRenderer);
	ImGui_ImplSDLRenderer3_Init(app.menuRenderer);




#ifdef __EMSCRIPTEN__
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (running)
#endif
	{ 
		ClearScreen(app.renderer);
 
		handleCollisionsOrig(app.gameData.entityNumber, x, y, dirX, dirY, w, h);
		//handleCollisionsNeigh(app.gameData.entityNumber, x, y, dirX, dirY, w, h);
		moveEntity(app.gameData.entityNumber, x, y, w, h, dirX, dirY);
		checkOutOfFrame(app.gameData.entityNumber, x, y, w, h, dirX, dirY);
		//collision(app.gameData.nEntity, x, y, dirX, dirY, w, h);
		

		// keyboard events
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				running = false;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(app.menuWindow))
				running = false;

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

		drawEntities(app.gameData.entityNumber, x, y, w, h, app.renderer);
		

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

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		static bool showGrid = false;
		{
			
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2((int)(500 * app.main_scale), (int)(250 * app.main_scale)), ImGuiCond_Always);


			ImGui::Begin("Modify game data");               

			ImGui::Text("Change game parameters");   

			ImGui::SliderInt("Number of entities", &app.gameData.entityNumber, 0, 100000);

			ImGui::Checkbox("Show grid", &showGrid);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

			ImGui::End();
		}

		if (showGrid) {
			drawGrid(app.renderer);

		}
		

		// Rendering
		ImGui::Render();
		SDL_SetRenderScale(app.menuRenderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_RenderClear(app.menuRenderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), app.menuRenderer);
		SDL_RenderPresent(app.menuRenderer);
	

		SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255);
		SDL_RenderDebugText(app.renderer, 10, 10, fps);
		SDL_RenderDebugText(app.renderer, 10, 20, avg);
		SDL_RenderDebugText(app.renderer, 10, 30, ms);


		SDL_RenderPresent(app.renderer);


	}

#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	// [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppQuit() function]
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(app.menuRenderer);
	SDL_DestroyWindow(app.menuWindow);
	SDL_Quit();

	delete[] x;
	delete[] y;
	delete[] w;
	delete[] h;
	delete[] dirX;
	delete[] dirY;


	ShutdownApplication();

	return EXIT_SUCCESS;

}

