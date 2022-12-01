#include "stdafx.h"
#include "MyGame.h"

#pragma warning(disable:4244)
float xPos, yPos, type;

CMyGame::CMyGame(void) :
	m_background("Ocean.png")
{
}

CMyGame::~CMyGame(void)
{
	// TODO: add destruction code here
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	if (!IsGameMode() || fishList.size() == 0) return;

	Uint32 t = GetTime();

	float fishSpeed = 120;
	float fishWanderSpeed = 30;

	CVector steerFish;
								//FISH AI START
	for (CFish* fish : fishList)
	{
		//CODE FOR PINK FISH
		if ((string)fish->GetProperty("tag") == "pink")
		{
			//INTERACTION WITH GOLD FISH
			for (CFish* target : fishList)
			{
				if ((string)target->GetProperty("tag") == "gold")
				{
					if (Distance(fish->GetPosition(), target->GetPosition()) <= 50)
					{
						steerFish = fish->flee(target->GetPosition(), fishSpeed);
						fish->Accelerate(steerFish * (float)GetDeltaTime() / 1000.f);
						break;
					}
				}
			}
		}
		//End of PINK FISH

        //CODE FOR SILVER FISH
		if ( ((string)fish->GetProperty("tag") == "silver")  &&  (fish->GetState() == CFish::FULL) )
		{
			//INTERACTION WITH OTHER FISH
			for (CFish* target : fishList)
			{
				if ((string)target->GetProperty("tag") == "pink")
				{
					if (Distance(fish->GetPosition(), target->GetPosition()) < 100)
					{
						steerFish = fish->seek(target->GetPosition(), fishSpeed-20);
						fish->Accelerate(steerFish * (float)GetDeltaTime() / 1000.f);
						break;
					}
				}
			}
		}
		//END OF SILVER FISH

		//START OF GOLD FISH
		if ((string)fish->GetProperty("tag") == "gold" && fish->GetState() == CFish::HUNGRY)
		{
			//INTERACTION WITH OTHER FISH
			for (CFish* target : fishList)
			{
				if ((string)target->GetProperty("tag") == "pink")
				{
					if (Distance(fish->GetPosition(), target->GetPosition()) < 50)
					{
						steerFish = fish->seek(target->GetPosition(), fishSpeed+20);
						fish->Accelerate(steerFish * (float)GetDeltaTime() / 1000.f);
						break;
					}
				}
			}
			
			//INTERACTION WITH PLAYER
			
		}
		//END OF GOLD FISH\\

		//INTERACTION WITH PLAYER FOOD
		for (CSprite* food : fishFood)
		{
			if ((Distance(fish->GetPosition(), food->GetPosition()) <= 75) && (fish->GetState() == CFish::HUNGRY))
			{
				steerFish = fish->seek(food->GetPosition(), fishSpeed-20);
				fish->Accelerate(steerFish * (float)GetDeltaTime() / 1000.f);
				break;
			}
		}

		//Fish will wander if none of above are met
		steerFish = fish->wander(fishWanderSpeed); //wander(speed,distance,radius)
		fish->Accelerate(steerFish * (float)GetDeltaTime() / 1000.f);
	}
	//FISH AI END


	//SPRITE UPDATING
	for (CFish* fish : fishList)	fish->Update(t);
	for (CSprite* food : fishFood)	food->Update(t);


	//HIT TEST FOR INTERACTION BETWEEN FISH
	for (CFish* fish : fishList)
	{
		//GOLD FISH INTERACTION
		if ( ((string)fish->GetProperty("tag") == "gold")  &&  (fish->GetState() == CFish::HUNGRY) )
		{
			for (CFish* target : fishList)
			{
				if ((fish->HitTest(target)) && ((string)target->GetProperty("tag") == "pink"))
				{
					target->ChangeState(CFish::DIE);
					fish->ChangeState(CFish::FULL);
					break;
				}
			}
		}
		//SILVER FISH INTERACTION
		if (((string)fish->GetProperty("tag") == "silver") && (fish->GetState() == CFish::FULL))
		{
			for (CFish* target : fishList)
			{
				if ((fish->HitTest(target)) && ((string)target->GetProperty("tag") == "pink"))
				{
					xPos = fish->GetX();
					yPos = fish->GetY();
					fishList.push_back(new CFish(xPos, yPos, "Pink_Fish.bmp", 0, "1"));
					fish->ChangeState(CFish::HUNGRY);
					break;
				}
			}
		}
	}

	//HIT TEST FOR INTERACTION WITH PLAYER FOOD
	for (CFish* fish : fishList)
	{
		for (CSprite* food : fishFood)
		{
			if ( (fish->HitTest(food)) && (fish->GetState() == CFish::HUNGRY) ) {
				fish->ChangeState(CFish::FULL);
				food->SetHealth(food->GetHealth() - 1);
			}
		}
	}


	
	//Removing dead fish
	for (CFish* fish : fishList)
	{
		if (fish->GetState() == CFish::DIE)
		{
			fish->Delete();
		}
	}
	//Removing eaten food
	for (CSprite* food : fishFood)
	{
		if (food->GetHealth() <= 0)	food->Delete();
	}

	fishList.delete_if(deleted);
	fishFood.delete_if(deleted);

	// Screen Movement for Fish
	extern CGameApp app;
	for (CFish* fish : fishList)
	{
		fish->wrapToScreen(app.GetWidth(), app.GetHeight());
	}
}

void DrawHealth(CGraphics* g, CVector pos, float w, float h, float health)
{
	if (health <= 0) return;
	g->DrawRect(CRectangle(pos.m_x - 1, pos.m_y - 1, w + 1, h + 1), CColor::Black());
	g->FillRect(CRectangle(pos.m_x, pos.m_y - 1, ceil(w * health / 100.f), h), health > 35 ? CColor::DarkGreen() : CColor::Red());
}

void CMyGame::OnDraw(CGraphics* g)
{
	// Draw background
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			g->Blit(CVector(256.f*i, 256.f *j), m_background);

	//Drawing Fish School
	for (CFish* fish : fishList)
	{
		fish->Draw(g);
		DrawHealth(g, fish->GetPosition() + CVector(-32, 32), 20, 4, fish->GetHealth());
	}
	//Drawing Player Fishfood
	for (CSprite* food : fishFood)	food->Draw(g);
/*
	if (IsGameOver())
		if (m_player.GetHealth() <= 0)
			* g << font(48) << color(CColor::DarkRed()) << vcenter << center << "GAME OVER" << endl;
		else
			*g << font(48) << color(CColor::DarkBlue()) << vcenter << center << "YOU'RE A SPIDER KILLER!" << endl;
*/
}

/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
}

// called when a new game is requested (e.g. when F2 pressed)
// use this function to prepare a menu or a welcome screen
void CMyGame::OnDisplayMenu()
{
	StartGame();	// exits the menu mode and starts the game mode
}

// called when a new game is started
// as a second phase after a menu or a welcome screen
void CMyGame::OnStartGame()
{
	fishList.delete_all();
	fishFood.delete_all();

	//MAKE 5 of each fish PINK FISH
	for (int i = 0; i < 5; i++)
	{
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Pink_Fish.bmp", 0, "1"));
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Silver_Fish.bmp", 0, "2"));
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Gold_Fish.bmp", 0, "3"));
	}
}

// called when a new level started - first call for nLevel = 1
void CMyGame::OnStartLevel(Sint16 nLevel)
{
}

// called when the game is over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();

	/*Key Commands!
	 1 - Spawn a Pink Fish
	 2 - Spawns a Silver Fish
	 3 - Spawns a Gold Fish
	*/
	if (sym == SDLK_1) {
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Pink_Fish.bmp", 0, "1"));
	}	
	if (sym == SDLK_2) {
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Silver_Fish.bmp", 0, "2"));
	}
	if (sym == SDLK_3) {
		xPos = float(rand() % 750);
		yPos = float(rand() % 550);
		fishList.push_back(new CFish(xPos, yPos, "Gold_Fish.bmp", 0, "3"));
	}


}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}

/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	pointer = new CSprite(x, y, "food.bmp", CColor::White(), GetTime());
	pointer->SetHealth(3);
	fishFood.push_back(pointer);
}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
