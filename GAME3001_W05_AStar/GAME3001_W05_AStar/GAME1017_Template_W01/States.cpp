#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include "TextureManager.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include "MathManager.h"
#include "EventManager.h"
#include "PathManager.h"
#include "DebugManager.h"
#include "SoundManager.h"

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

PlayState::PlayState() {}

void PlayState::Enter()
{
	std::cout << "playsucces" << std::endl;
	
	m_pPlayer = new Player({ 0,0,43,58 }, { (float)(16) * 32, (float)(12) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("ship"), 0, 0, 0, 1);
	m_pBling = new Sprite({ 224,64,32,32 }, { (float)(16) * 32, (float)(4) * 32, 32, 32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("tiles"));
	m_directions = new Label("tile2", 0, 780, " F -Find the shorest path || H -Trigger Debug ||Right click(Debug) -Move Player || +/- -volume ", { 255, 255, 255, 255 });
	m_directions2 = new Label("tile2", 0, 800, " M -Player will automatically move to goal || Space -Switch Heuristic || Left click(Debug) -Move Goal || R -Reset to Start(Moving must finish) ", { 255, 255, 255, 255 });
	m_b = "Total cost: ";
	m_cost = new Label("tile2", 900, 780, m_b, { 255, 255, 255, 255 });
	SOMA::SetMusicVolume(Engine::Instance().getvol());
	SOMA::SetSoundVolume(Engine::Instance().getvol());
	SOMA::PlayMusic("PBGM");
	std::ifstream inFile("Dat/Waterdata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			Engine::Instance().GetTiles().emplace(key, new Tile({ x * 48, y * 48, 48, 48 }, { 0,0,32,32 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("watertiles"), o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Water.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				inFile >> key;
				Engine::Instance().GetLevel()[row][col] = Engine::Instance().GetTiles()[key]->Clone(); // Prototype design pattern used.
				Engine::Instance().GetLevel()[row][col]->GetDstP()->x = (float)(32 * col);
				Engine::Instance().GetLevel()[row][col]->GetDstP()->y = (float)(32 * row);
				// Instantiate the labels for each tile.
				Engine::Instance().GetLevel()[row][col]->m_lCost = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 4, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 18, " ", { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lX = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 18, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, std::to_string(col).c_str(), { 0,0,0,255 });
				Engine::Instance().GetLevel()[row][col]->m_lY = new Label("tile", Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 2, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 2, std::to_string(row).c_str(), { 0,0,0,255 });
				// Construct the Node for a valid tile.
				if (!Engine::Instance().GetLevel()[row][col]->IsObstacle() || Engine::Instance().GetLevel()[row][col]->IsHazard())
					Engine::Instance().GetLevel()[row][col]->m_node = new PathNode((int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->x), (int)(Engine::Instance().GetLevel()[row][col]->GetDstP()->y));
			}
		}
	}
	inFile.close();
	// Now build the graph from ALL the non-obstacle and non-hazard tiles. Only N-E-W-S compass points.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{ 
			if (Engine::Instance().GetLevel()[row][col]->Node() == nullptr) // Now we can test for nullptr.
				continue; // An obstacle or hazard tile has no connections.
			// Make valid connections.
			if (row - 1 != -1 && !Engine::Instance().GetLevel()[row - 1][col]->IsObstacle() && !Engine::Instance().GetLevel()[row - 1][col]->IsHazard())
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row - 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row-1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row-1][col]->Node()->y)));
			if (row + 1 != ROWS && !Engine::Instance().GetLevel()[row + 1][col]->IsObstacle() && !Engine::Instance().GetLevel()[row + 1][col]->IsHazard())
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row + 1][col]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row + 1][col]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row + 1][col]->Node()->y)));
			if (col - 1 != -1 && !Engine::Instance().GetLevel()[row][col - 1]->IsObstacle() && !Engine::Instance().GetLevel()[row][col - 1]->IsHazard())
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col - 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col - 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col - 1]->Node()->y)));
			if (col + 1 != COLS && !Engine::Instance().GetLevel()[row][col + 1]->IsObstacle() && !Engine::Instance().GetLevel()[row][col + 1]->IsHazard())
				Engine::Instance().GetLevel()[row][col]->Node()->AddConnection(new PathConnection(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[row][col + 1]->Node(),
					MAMA::Distance(Engine::Instance().GetLevel()[row][col]->Node()->x, Engine::Instance().GetLevel()[row][col + 1]->Node()->x, Engine::Instance().GetLevel()[row][col]->Node()->y, Engine::Instance().GetLevel()[row][col + 1]->Node()->y)));
		}
	}
	
}
void PlayState::Update()
{
	
	
	if (m_moving)
	{
		++m_frameCounter;
		if (m_count == PAMA::PathList().size() - 1 && m_frameCounter == 50 * (PAMA::PathList().size() + 1))
		{
			m_pPlayer->GetDstP()->x = m_pBling->GetDstP()->x;
			m_pPlayer->GetDstP()->y = m_pBling->GetDstP()->y;
			m_moving = false;
			m_frameCounter = 0;
			m_count = 0;
			m_shortPath = false;

		}
		for (unsigned i = 1; i < PAMA::PathList().size(); ++i)
		{
			if (m_frameCounter == 50 * i)
			{
				++m_count;
				m_a = std::to_string(m_count);
				m_c = m_b + m_a;
				m_cost->SetText(m_c);
                SOMA::PlaySound("move",0,2);
			}
		}
		
		if (m_moving)
		{
			if (m_pPlayer->GetDstP()->x > PAMA::PathList()[m_count]->GetFromNode()->Pt().x)
			{
				m_pPlayer->GetSrcP()->y = 58;
				m_pPlayer->GetSrcP()->w = 75;
				m_pPlayer->GetSrcP()->h = 53;
				m_pPlayer->GetDir(0);
			}
			else if (m_pPlayer->GetDstP()->x < PAMA::PathList()[m_count]->GetFromNode()->Pt().x)
			{
				m_pPlayer->GetSrcP()->y = 58;
				m_pPlayer->GetSrcP()->w = 75;
				m_pPlayer->GetSrcP()->h = 53;
				m_pPlayer->GetDir(1);
			}
			else if (m_pPlayer->GetDstP()->y < PAMA::PathList()[m_count]->GetFromNode()->Pt().y)
			{
				m_pPlayer->GetSrcP()->y = 0;
				m_pPlayer->GetSrcP()->w = 43;
				m_pPlayer->GetSrcP()->h = 58;
				m_pPlayer->GetDir(0);
			}
			else if (m_pPlayer->GetDstP()->y > PAMA::PathList()[m_count]->GetFromNode()->Pt().y)
			{
				m_pPlayer->GetSrcP()->y = 0;
				m_pPlayer->GetSrcP()->w = 43;
				m_pPlayer->GetSrcP()->h = 58;
				m_pPlayer->GetDir(2);
			}
			m_pPlayer->GetDstP()->x = PAMA::PathList()[m_count]->GetFromNode()->Pt().x;
			m_pPlayer->GetDstP()->y = PAMA::PathList()[m_count]->GetFromNode()->Pt().y;
			
			std::cout << "frame" << m_frameCounter << std::endl;
			
			std::cout << "count" << m_count << std::endl;
			std::cout << "size" << PAMA::PathList().size() << std::endl;
		}
		
	}
    
	if (EVMA::KeyPressed(SDL_SCANCODE_R) && !m_moving)
	{
		m_pPlayer->setDesX(PAMA::PathList()[0]->GetFromNode()->Pt().x);
		m_pPlayer->setDesY(PAMA::PathList()[0]->GetFromNode()->Pt().y);
	}

	if (EVMA::KeyPressed(SDL_SCANCODE_H)) // ~ or ` key. Toggle debug mode.
	{

		m_showCosts = !m_showCosts;
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_F) && !m_moving)
	{
		PAMA::GetShortestPath(Engine::Instance().GetLevel()[(int)(m_pPlayer->GetDstP()->y / 32)][(int)(m_pPlayer->GetDstP()->x / 32)]->Node(),
			Engine::Instance().GetLevel()[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node());
		m_shortPath = !m_shortPath;
	}

	//m_pPlayer->Update(); // Just stops MagaMan from moving.
	if (!m_moving && m_showCosts)
	{
		
		if (EVMA::KeyPressed(SDL_SCANCODE_SPACE)) // Toggle the heuristic used for pathfinding.
		{

			m_hEuclid = !m_hEuclid;
			std::cout << "Setting " << (m_hEuclid ? "Euclidian" : "Manhattan") << " heuristic..." << std::endl;
		}
		if (EVMA::MousePressed(1) || EVMA::MousePressed(3)) // If user has clicked.
		{
			int xIdx = (EVMA::GetMousePos().x / 32);
			int yIdx = (EVMA::GetMousePos().y / 32);
			if (Engine::Instance().GetLevel()[yIdx][xIdx]->IsObstacle() || Engine::Instance().GetLevel()[yIdx][xIdx]->IsHazard())
				return; // We clicked on an invalid tile.
			

			if (EVMA::MousePressed(1)) // Move the player with left-click.
			{	
				if (EVMA::GetMousePos().x >= m_pBling->GetDstP()->x && EVMA::GetMousePos().x <= m_pBling->GetDstP()->x + 32
					&& EVMA::GetMousePos().y >= m_pBling->GetDstP()->y && EVMA::GetMousePos().y <= m_pBling->GetDstP()->y + 32)
					return;
				m_pPlayer->GetDstP()->x = (float)(xIdx * 32);
				m_pPlayer->GetDstP()->y = (float)(yIdx * 32);
			}
			else if (EVMA::MousePressed(3)) // Else move the bling with right-click.
			{
				if (EVMA::GetMousePos().x >= m_pPlayer->GetDstP()->x && EVMA::GetMousePos().x <= m_pPlayer->GetDstP()->x + 32
					&& EVMA::GetMousePos().y >= m_pPlayer->GetDstP()->y && EVMA::GetMousePos().y <= m_pPlayer->GetDstP()->y + 32)
					return;
				m_pBling->GetDstP()->x = (float)(xIdx * 32);
				m_pBling->GetDstP()->y = (float)(yIdx * 32);
			}
			for (int row = 0; row < ROWS; row++) // "This is where the fun begins."
			{ // Update each node with the selected heuristic and set the text for debug mode.
				for (int col = 0; col < COLS; col++)
				{
					if (Engine::Instance().GetLevel()[row][col]->Node() == nullptr)
						continue;
					if (m_hEuclid)
						Engine::Instance().GetLevel()[row][col]->Node()->SetH(PAMA::HEuclid(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
					else
						Engine::Instance().GetLevel()[row][col]->Node()->SetH(PAMA::HManhat(Engine::Instance().GetLevel()[row][col]->Node(), Engine::Instance().GetLevel()[(int)(m_pBling->GetDstP()->y / 32)][(int)(m_pBling->GetDstP()->x / 32)]->Node()));
					Engine::Instance().GetLevel()[row][col]->m_lCost->SetText(std::to_string((int)(Engine::Instance().GetLevel()[row][col]->Node()->H())).c_str());
				}
			}
			// Now we can calculate the path. Note: I am not returning a path again, only generating one to be rendered.

		}
		
	}
	if (EVMA::KeyHeld(SDL_SCANCODE_M) && m_shortPath)
	{
		m_moving = true;
	}
	if (EVMA::KeyHeld(SDL_SCANCODE_MINUS))
	{
		SOMA::SetAllVolume(Engine::Instance().setvol(-1));
		std::cout << Engine::Instance().getvol() << std::endl;
	}
	else if (EVMA::KeyHeld(SDL_SCANCODE_EQUALS))
	{
		SOMA::SetAllVolume(Engine::Instance().setvol(1));
		std::cout << Engine::Instance().getvol() << std::endl;
	}
}

void PlayState::Render()
{
	// Draw anew.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			Engine::Instance().GetLevel()[row][col]->Render(); // Render each tile.
			// Render the debug data...
			if (m_showCosts && Engine::Instance().GetLevel()[row][col]->Node() != nullptr)
			{
				Engine::Instance().GetLevel()[row][col]->m_lCost->Render();
				Engine::Instance().GetLevel()[row][col]->m_lX->Render();
				Engine::Instance().GetLevel()[row][col]->m_lY->Render();
				// I am also rendering out each connection in blue. If this is a bit much for you, comment out the for loop below.
				for (unsigned i = 0; i < Engine::Instance().GetLevel()[row][col]->Node()->GetConnections().size(); i++)
				{
					DEMA::QueueLine({ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->x + 16, 
						Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetFromNode()->y + 16 },
						{ Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->x + 16, 
						Engine::Instance().GetLevel()[row][col]->Node()->GetConnections()[i]->GetToNode()->y + 16 }, { 0,0,255,255 });
				}
			}
			
		}
	}
	m_pPlayer->Render();
	m_pBling->Render();
	
	m_directions->Render();
	m_directions2->Render();
	m_cost->Render();
	if(m_shortPath)
		PAMA::DrawPath(); // I save the path in a static vector to be drawn here.
	DEMA::FlushLines(); // And... render ALL the queued lines. Phew.
	// Flip buffers.
}



void PlayState::Exit()
{
}

void PlayState::Resume()
{
}

TitleState::TitleState() {}

void TitleState::Enter()
{
	std::cout << "titlesucces" << std::endl;
	m_playBtn = new PlayButton({ 0, 0,200,80 }, { 412, 344 ,200,80 }, Engine::Instance().GetRenderer(), TEMA::GetTexture("start"));
	
	std::string c = "Michael Shular 101273089", d = "Daekoen Lee 101076401";
	m_nameOne = new Label("tile3", 420, 180, c, { 255, 255, 255, 255 });
	m_nameTwo = new Label("tile3", 420, 140, d, { 255, 255, 255, 255 });
}

void TitleState::Update()
{
	if (m_playBtn->Update() == 1)
		return;
	
}


void TitleState::Render()
{
	m_playBtn->Render();
	State::Render();
	
	m_nameOne->Render();
	m_nameTwo->Render();
}



void TitleState::Exit()
{
	std::cout << "Exiting TitleState..." << std::endl;
}
