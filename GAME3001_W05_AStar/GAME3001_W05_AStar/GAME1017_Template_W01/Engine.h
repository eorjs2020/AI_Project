#pragma once
#ifndef _ENGINE_H_
#define _ENGINE_H_

// Do not include any macros, initialized properties or full function definitions in this header.

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "Tile.h"
#include "Player.h"
#include <array>
#include <map>

#define ROWS 24
#define COLS 32

class Engine
{
public: // Public methods.
	int Run();
	static Engine& Instance(); // This static method creates the static instance that can be accessed 'globally'
	SDL_Renderer* GetRenderer();
	bool& Running();
	std::array<std::array<Tile*, COLS>, ROWS>& GetLevel() { return m_level; }
	std::map<char, Tile*>& GetTiles() { return m_tiles; }
	int& setvol(int v);
	int& getvol();

private: // Private properties.
	bool m_running; // Loop control flag.
	Uint32 m_start, m_end, m_delta, m_fps; // Fixed timestep variables.
	SDL_Window* m_pWindow; // This represents the SDL window.
	SDL_Renderer* m_pRenderer; // This represents the buffer to draw to.
	// Example-specific properties.
	SDL_Texture *m_pTileText, *m_pPlayerText;
	std::map<char, Tile*> m_tiles;
	std::array<std::array<Tile*, COLS>,ROWS> m_level; // Fixed-size STL array of Tile pointers.
	Player* m_pPlayer = nullptr;
	Sprite* m_pBling;
	bool m_showCosts = false, m_hEuclid = true;
	int m_volControl = 15;

private: // Private methods.
	Engine();
	bool Init(const char* title, int xpos, int ypos, int width, int height, int flags);
	void Wake();
	void Sleep();
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
};

#endif