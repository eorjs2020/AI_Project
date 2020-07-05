#include "Engine.h"
#include "CollisionManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "FontManager.h"
#include "PathManager.h"
#include "SoundManager.h"
#include "StateManager.h"
#include "TextureManager.h"
#include <iostream>
#include <fstream>
#include <ctime>
#define WIDTH 1024
#define HEIGHT 820
#define FPS 60
using namespace std;

Engine::Engine():m_running(false){ cout << "Engine class constructed!" << endl; }

bool Engine::Init(const char* title, int xpos, int ypos, int width, int height, int flags)
{
	cout << "Initializing game..." << endl;
	// Attempt to initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		// Create the window.
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (m_pWindow != nullptr) // Window init success.
		{
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
			if (m_pRenderer != nullptr) // Renderer init success.
			{
				EVMA::Init();
				SOMA::Init();
				TEMA::Init();
			}
			else return false; // Renderer init fail.
		}
		else return false; // Window init fail.
	}
	else return false; // SDL init fail.
	// Example specific initialization.
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2"); // Call this before any textures are created.
	srand((unsigned)time(NULL));
	m_pTileText = IMG_LoadTexture(m_pRenderer, "Img/Tiles.png");
	m_pPlayerText = IMG_LoadTexture(m_pRenderer, "Img/Maga.png");
	TEMA::RegisterTexture("Img/start.png", "start");
	TEMA::RegisterTexture("Img/Maga.png", "maga");
	TEMA::RegisterTexture("Img/Tiles.png", "tiles");
	TEMA::RegisterTexture("Img/Ship.png", "ship");
	TEMA::RegisterTexture("Img/pipo_map.png", "maptiles");
	TEMA::RegisterTexture("Img/pipo_water.png", "watertiles");
	FOMA::RegisterFont("Img/ltype.ttf", "tile", 10);
	FOMA::RegisterFont("Img/ltype.ttf", "tile2", 12);
	FOMA::RegisterFont("Img/ltype.ttf", "tile3", 20);
	SOMA::Load("Aud/water-theme.wav", "PBGM", SOUND_MUSIC);
	SOMA::Load("Aud/plop.wav", "move", SOUND_SFX);
	STMA::ChangeState(new TitleState);
	SOMA::AllocateChannels(16);

	// Final engine initialization calls.
	m_fps = (Uint32)round((1 / (double)FPS) * 1000); // Sets FPS in milliseconds and rounds.
	m_running = true; // Everything is okay, start the engine.
	cout << "Engine Init success!" << endl;
	return true;
}

void Engine::Wake()
{
	m_start = SDL_GetTicks();
}

void Engine::Sleep()
{
	m_end = SDL_GetTicks();
	m_delta = m_end - m_start;
	if (m_delta < m_fps) // Engine has to sleep.
		SDL_Delay(m_fps - m_delta);
}

void Engine::HandleEvents()
{
	EVMA::HandleEvents();
}

void Engine::Update()
{

	STMA::Update();
	
}

void Engine::Render() 
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer); // Clear the screen with the draw color.
	STMA::Render();
								
	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	cout << "Cleaning game." << endl;
	// Example-specific cleaning.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			delete m_level[row][col];
			m_level[row][col] = nullptr; // Wrangle your dangle.
		}
	}
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
	// Finish cleaning.
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	DEMA::Quit();
	EVMA::Quit();
	FOMA::Quit();
	SOMA::Quit();
	STMA::Quit();
	TEMA::Quit();
	IMG_Quit();
	SDL_Quit();
}

int Engine::Run()
{
	if (m_running) // What does this do and what can it prevent?
		return -1; 
	if (Init("GAME1017 Engine Template", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0) == false)
		return 1;
	while (m_running) // Main engine loop.
	{
		Wake();
		HandleEvents();
		Update();
		Render();
		if (m_running)
			Sleep();
	}
	Clean();
	return 0;
}

Engine& Engine::Instance()
{
	static Engine instance; // C++11 will prevent this line from running more than once. Magic statics.
	return instance;
}

SDL_Renderer* Engine::GetRenderer() { return m_pRenderer; }
bool& Engine::Running() { return m_running; }

int& Engine::setvol(int v)
{
	m_volControl = m_volControl + v;
	return m_volControl;
}

int& Engine::getvol()
{
	return m_volControl;
}
