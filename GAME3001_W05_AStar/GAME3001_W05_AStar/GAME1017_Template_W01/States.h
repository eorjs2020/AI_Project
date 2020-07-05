#pragma once
#ifndef _STATES_H_
#define _STATES_H_
#define ROWS 24
#define COLS 32
#define WIDTH 1024
#define HEIGHT 768

#include <SDL.h>
#include <SDL_image.h>
#include "Button.h"
#include "Label.h"
#include <array>
#include <map>
#include "Tile.h"
#include "Player.h"


class State // This is the abstract base class for all specific states.
{
public: // Public methods.
	virtual void Update() = 0; // Having at least one 'pure virtual' method like this, makes a class abtract.
	virtual void Render();     // Meaning we cannot create objects of the class.
	virtual void Enter() = 0;  // Virtual keyword means we can override in derived class.
	virtual void Exit() = 0;
	virtual void Resume();

protected: // Private but inherited.
	State() {}
};

class PlayState : public State
{
private:
	int m_count = 0;
	int m_frameCounter = 0;
	SDL_Texture* m_pTileText, * m_pPlayerText;
	Label* m_directions, * m_cost;
	Player* m_pPlayer;
	Sprite* m_pBling;
	bool m_showCosts = false, m_hEuclid = true, m_moving = false;
public:
	PlayState();
	void Update();
	void Render();
	void Enter();
	void Exit();
	void Resume();
};

class TitleState : public State
{
private:
	Label* m_nameOne, * m_nameTwo;
	Button* m_playBtn;
public:
	TitleState();
	void Update();
	void Render();
	void Enter();
	void Exit();
};

#endif