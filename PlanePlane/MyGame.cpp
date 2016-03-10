// Beginning Game Programming
// MyGame.cpp

#include "MyDirectX.h"
#include <sstream>
#include <list>
using namespace std;

const string APPTITLE = "Plane Plane";
const int SCREENW = 600;
const int SCREENH = 800;

LPD3DXFONT font;

bool host = false;

const DWORD msPerFrame = 14;

//gameWorld settings
const int TILEWIDTH = 64;
const int TILEHEIGHT = 64;
const int MAPWIDTH = 25;
const int MAPHEIGHT = 18;

//scrolling window size
const int WINDOWWIDTH = (SCREENW / TILEWIDTH) * TILEWIDTH;
const int WINDOWHEIGHT = (SCREENH / TILEHEIGHT) * TILEHEIGHT;

//entire game world dimensions
const int gameWorldWIDTH = TILEWIDTH * MAPWIDTH;
const int gameWorldHEIGHT = TILEHEIGHT * MAPHEIGHT;

int ScrollX, ScrollY;
int SpeedX, SpeedY;
LPDIRECT3DSURFACE9 gameWorld = NULL;

long score = 0;

// texture
LPDIRECT3DTEXTURE9 playerImage = NULL;
LPDIRECT3DTEXTURE9 bulletsImage = NULL;
LPDIRECT3DTEXTURE9 hostileImage = NULL;
LPDIRECT3DTEXTURE9 boomImage = NULL;

// player
SPRITE player;
int playerLife = 1;
bool playerVisible = false;

// 500.0 pixle per second speed
const float playerPixPerSecond = 250.0;
const float friendlyBulletPixPerSecond = 400.0;
DWORD playerShootingInterval = 400;
DWORD animationDelay = 60;
DWORD slowAnimationDelay = 120;

float minimumHostilePixPerSecond = 20.0;
float maximumHostilePixPerSecond = 80.0;
DWORD msPerHostile = 1000.0;
DWORD msPerUpgrade = 5000.0;
// bullets
list<SPRITE> friendlyBullets;
list<SPRITE> friendlyPlanes;
list<SPRITE> hostileBullets;
list<SPRITE> hostilePlanes;
list<SPRITE> booms;

DWORD screenTimer = 0;
DWORD shootTimer = 0;
DWORD hostileTimer = 0;
DWORD hostileUpgradeTimer = 0;

DWORD dt;

int MAPDATA[MAPWIDTH*MAPHEIGHT] = {
	80,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,
	81,81,81,82,90,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,92,3,3,3,3,3,92,3,
	92,90,3,13,83,96,3,3,23,3,92,3,13,92,3,3,3,3,3,3,11,3,13,3,3,92,
	90,3,3,3,3,3,3,3,10,3,3,3,3,3,23,3,3,3,3,3,3,3,13,3,92,90,3,96,
	3,13,3,3,3,3,3,3,3,3,3,3,3,3,96,3,23,3,96,3,3,92,90,3,3,3,3,3,3,
	13,3,3,3,13,3,3,11,3,3,3,3,3,3,3,13,3,92,90,3,83,11,3,92,3,3,3,
	3,3,11,3,3,3,3,3,3,3,83,3,3,3,92,92,90,3,3,3,96,3,13,3,3,3,11,
	10,3,3,3,3,3,13,3,3,13,3,3,3,92,90,3,23,3,3,3,3,3,3,96,3,3,83,
	3,3,3,92,3,3,3,3,3,13,3,92,90,3,3,3,3,3,3,3,3,3,3,3,3,23,3,3,3,
	3,3,3,3,3,3,3,92,90,3,3,3,11,3,92,3,3,13,3,3,131,3,10,3,3,3,96,
	3,92,3,96,3,92,90,3,13,83,3,3,3,3,3,3,3,3,3,3,3,13,3,3,3,3,3,3,
	3,3,92,90,3,3,3,3,13,3,3,3,3,3,11,96,3,3,3,3,3,3,13,3,13,3,11,
	92,90,92,3,13,3,3,3,3,3,3,92,3,10,3,23,3,3,3,3,3,3,3,3,3,92,90,
	3,3,3,3,3,96,3,23,3,3,3,3,3,3,3,3,83,3,3,13,3,96,3,92,90,3,3,3,
	3,92,3,3,3,3,3,13,3,3,3,13,3,3,3,11,3,3,3,3,92,90,3,13,3,3,3,3,
	3,3,3,96,3,3,3,3,3,3,3,3,3,3,92,3,3,92,100,101,101,101,101,101,
	101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,
	101,101,102
};

void DrawTile(LPDIRECT3DSURFACE9 source,	// source surface image
		int tilenum,				// tile #
		int width,					// tile width
		int height,					// tile height
		int columns,				// columns of tiles
		LPDIRECT3DSURFACE9 dest,	// destination surface
		int destx,					// destination x
		int desty)					// destination y
{

		//create a RECT to describe the source image
		RECT r1;
		r1.left = (tilenum % columns) * width;
		r1.top = (tilenum / columns) * height;
		r1.right = r1.left + width;
		r1.bottom = r1.top + height;

		//set destination rect
		RECT r2 = { destx,desty,destx + width,desty + height };

		//draw the tile 
		d3ddev->StretchRect(source, &r1, dest, &r2, D3DTEXF_NONE);
}

void BuildgameWorld()
{
		HRESULT result;
		int x, y;
		LPDIRECT3DSURFACE9 tiles;

		//load the bitmap image containing all the tiles
		tiles = LoadSurface("groundtiles.bmp");

		//create the scrolling game world bitmap
		result = d3ddev->CreateOffscreenPlainSurface(
				gameWorldWIDTH,         //width of the surface
				gameWorldHEIGHT,        //height of the surface
				D3DFMT_X8R8G8B8,
				D3DPOOL_DEFAULT,
				&gameWorld,             //pointer to the surface
				NULL);

		if (result != D3D_OK)
		{
				MessageBox(NULL, "Error creating working surface!", "Error", 0);
				return;
		}

		//fill the gameWorld bitmap with tiles
		for (y = 0; y < MAPHEIGHT; y++)
				for (x = 0; x < MAPWIDTH; x++)
						DrawTile(tiles, MAPDATA[y * MAPWIDTH + x], 64, 64, 16,
								gameWorld, x * 64, y * 64);

		//now the tiles bitmap is no longer needed
		tiles->Release();
}

void ScrollScreen()
{
		//update horizontal scrolling position and speed
		ScrollX += SpeedX;
		if (ScrollX < 0)
		{
				ScrollX = 0;
				SpeedX = 0;
		} else if (ScrollX > gameWorldWIDTH - SCREENW)
		{
				ScrollX = gameWorldWIDTH - SCREENW;
				SpeedX = 0;
		}

		//update vertical scrolling position and speed
		ScrollY += SpeedY;
		if (ScrollY < 0)
		{
				ScrollY = 0;
				SpeedY = 0;
		} else if (ScrollY > gameWorldHEIGHT - SCREENH)
		{
				ScrollY = gameWorldHEIGHT - SCREENH;
				SpeedY = 0;
		}

		//set dimensions of the source image
		RECT r1 = { ScrollX, ScrollY, ScrollX + SCREENW - 1, ScrollY + SCREENH - 1 };

		//set the destination rect
		RECT r2 = { 0, 0, SCREENW - 1, SCREENH - 1 };

		//draw the current game world view
		d3ddev->StretchRect(gameWorld, &r1, backbuffer, &r2,
				D3DTEXF_NONE);

}

void BOOMCheck(DWORD now)
{
		// hit enemy
		for (list<SPRITE>::iterator it1 = friendlyBullets.begin(); it1 != friendlyBullets.end();)
		{
				bool hit = false;
				for (list<SPRITE>::iterator it2 = hostilePlanes.begin(); it2 != hostilePlanes.end();)
				{
						if (CollisionD(*it1, *it2))
						{
								// create boom
								SPRITE BOOM;
								BOOM.width = 32;
								BOOM.height = 32;
								BOOM.scaling = it2->scaling;
								BOOM.x = it2->x + it2->width * it2->scaling / 2 - BOOM.width * BOOM.scaling / 2;
								BOOM.y = it2->y + it2->height * it2->scaling / 2 - BOOM.height * BOOM.scaling / 2;
								BOOM.columns = 7;
								BOOM.starttime = now;
								BOOM.delay = animationDelay;
								BOOM.startframe = 0;
								BOOM.endframe = 6;
								booms.push_back(BOOM);

								it2 = hostilePlanes.erase(it2);
								hit = true;
								break;
						}
						it2++;
				}
				if (hit)
				{
						it1 = friendlyBullets.erase(it1);
						continue;
				}
				it1++;
		}

		// hit Allys
		for (list<SPRITE>::iterator it1 = hostilePlanes.begin(); it1 != hostilePlanes.end();)
		{
				bool hit = false;
				if (playerVisible && CollisionD(*it1, player))
				{
						// create boom
						SPRITE BOOM;
						BOOM.width = 32;
						BOOM.height = 32;
						BOOM.columns = 7;
						BOOM.starttime = now;
						BOOM.delay = animationDelay;
						BOOM.startframe = 0;
						BOOM.endframe = 6;
						BOOM.scaling = player.scaling;
						BOOM.x = player.x + player.width * player.scaling / 2 - BOOM.width * BOOM.scaling / 2;
						BOOM.y = player.y + player.height * player.scaling / 2 - BOOM.height * BOOM.scaling / 2;
						booms.push_back(BOOM);

						playerVisible = false;
						playerLife--;
						hit = true;
				}

				for (list<SPRITE>::iterator it2 = friendlyPlanes.begin(); it2 != friendlyPlanes.end();)
				{
						if (CollisionD(*it1, *it2))
						{
								// create boom
								SPRITE BOOM;
								BOOM.width = 32;
								BOOM.height = 32;
								BOOM.scaling = it2->scaling;
								BOOM.x = it2->x + it2->width * it2->scaling / 2 - BOOM.width * BOOM.scaling / 2;
								BOOM.y = it2->y + it2->height * it2->scaling / 2 - BOOM.height * BOOM.scaling / 2;
								BOOM.columns = 7;
								BOOM.starttime = now;
								BOOM.delay = animationDelay;
								BOOM.startframe = 0;
								BOOM.endframe = 6;
								booms.push_back(BOOM);

								it2 = hostilePlanes.erase(it2);
								hit = true;
								continue;
						}
						it2++;
				}

				if (hit)
				{
						// create boom
						SPRITE BOOM;
						BOOM.width = 32;
						BOOM.height = 32;
						BOOM.scaling = it1->scaling;
						BOOM.x = it1->x + it1->width * it1->scaling / 2 - BOOM.width * BOOM.scaling / 2;
						BOOM.y = it1->y + it1->height * it1->scaling / 2 - BOOM.height * BOOM.scaling / 2;
						BOOM.columns = 7;
						BOOM.starttime = now;
						BOOM.delay = animationDelay;
						BOOM.startframe = 0;
						BOOM.endframe = 6;
						booms.push_back(BOOM);
						it1 = hostilePlanes.erase(it1);
						continue;
				}
				it1++;
		}
}

void MoveSprites(DWORD now, DWORD dt)
{
		// move players
		player.x += player.velx * dt / 1000.0f;
		player.y += player.vely * dt / 1000.0f;
		Sprite_Animate(player.frame, player.startframe, player.endframe, player.direction, player.starttime, player.delay, now, true);
		// move friendly
		for (list<SPRITE>::iterator it = friendlyPlanes.begin(); it != friendlyPlanes.end(); ++it)
		{
				it->y += it->vely * dt / 1000.0f;
				it->x += it->velx * dt / 1000.0f;
		}

		// move firendly bullets
		for (list<SPRITE>::iterator it = friendlyBullets.begin(); it != friendlyBullets.end(); ++it)
		{
				it->y += it->vely * dt / 1000.0f;
				it->x += it->velx * dt / 1000.0f;
		}

		// move hostile planes
		for (list<SPRITE>::iterator it = hostilePlanes.begin(); it != hostilePlanes.end(); ++it)
		{
				it->y += it->vely * dt / 1000.0f;
				it->x += it->velx * dt / 1000.0f;
		}

		// move hostile bullets
		for (list<SPRITE>::iterator it = hostileBullets.begin(); it != hostileBullets.end(); ++it)
		{
				it->y += it->vely * dt / 1000.0f;
				it->x += it->velx * dt / 1000.0f;
		}

		for (list<SPRITE>::iterator it = booms.begin(); it != booms.end(); ++it)
		{
				Sprite_Animate(it->frame, it->startframe, it->endframe, it->direction, it->starttime, it->delay, now, true);
		}
}

void BoundsCheck()
{
		// check players
		if (player.x >= SCREENW - player.width * player.scaling) player.x = SCREENW - player.width * player.scaling;
		if (player.x <= 0) player.x = 0;
		if (player.y <= SCREENH / 2) player.y = SCREENH / 2;
		if (player.y >= SCREENH - player.height * player.scaling) player.y = SCREENH - player.height * player.scaling;

		for (list<SPRITE>::iterator it = friendlyPlanes.begin(); it != friendlyPlanes.end();)
		{
				if (it->x >= SCREENW - it->width * it->scaling) it->x = SCREENW - it->width * it->scaling;
				if (it->x <= 0) it->x = 0;
				if (it->y <= SCREENH / 2) it->y = SCREENH / 2;
				if (it->y >= SCREENH - it->height * it->scaling) it->y = SCREENH - it->height * it->scaling;
		}

		for (list<SPRITE>::iterator it = friendlyBullets.begin(); it != friendlyBullets.end();)
		{
				if (it->x > SCREENW || it->y > SCREENH || it->x < -it->width * it->scaling || it->y < -it->height * it->scaling)
				{
						it = friendlyBullets.erase(it);
						continue;
				}
				it++;
		}

		for (list<SPRITE>::iterator it = hostilePlanes.begin(); it != hostilePlanes.end();)
		{
				if (it->x > SCREENW || it->y > SCREENH || it->x < -it->width * it->scaling || it->y < -it->height * it->scaling)
				{
						it = hostilePlanes.erase(it);
						continue;
				}
				it++;
		}

		for (list<SPRITE>::iterator it = hostileBullets.begin(); it != hostileBullets.end();)
		{
				if (it->x > SCREENW || it->y > SCREENH || it->x < -it->width * it->scaling || it->y < -it->height * it->scaling)
				{
						it = hostileBullets.erase(it);
						continue;
				}
				it++;
		}

		for (list<SPRITE>::iterator it = booms.begin(); it != booms.end();)
		{
				if (it->frame == it->endframe)
				{
						it = booms.erase(it);
						continue;
				}
				it++;
		}
}

void HostileIncoming(DWORD now)
{
		if (now - hostileTimer >= msPerHostile)
		{
				hostileTimer = now;
				SPRITE newHostile;
				newHostile.width = 32;
				newHostile.height = 32;
				newHostile.scaling = 1.0 + (rand() % 6) / 10.0;
				newHostile.x = rand() % (int)(SCREENW - newHostile.width * newHostile.scaling);
				newHostile.y = -newHostile.height * newHostile.scaling;
				newHostile.frame = 0;
				newHostile.columns = 9;
				newHostile.startframe = 0;
				newHostile.endframe = 0;
				newHostile.vely = minimumHostilePixPerSecond + rand() % (int)maximumHostilePixPerSecond;
				hostilePlanes.push_back(newHostile);
		}
}

void HostileUpgrade(DWORD now)
{
		if (now - hostileUpgradeTimer >= msPerUpgrade)
		{
				hostileUpgradeTimer = now;
				if (msPerHostile > 100) msPerHostile - 50;
				maximumHostilePixPerSecond += 10;
				minimumHostilePixPerSecond += 2;
		}
}

void DrawSprites(DWORD now)
{
		// draw players
		if (playerVisible)
				Sprite_Transform_Draw(playerImage, player);
		// draw bullets
		for (list<SPRITE>::iterator it = friendlyBullets.begin(); it != friendlyBullets.end(); ++it)
		{
				Sprite_Transform_Draw(bulletsImage, *it);
		}

		for (list<SPRITE>::iterator it = friendlyPlanes.begin(); it != friendlyPlanes.end(); ++it)
		{
				Sprite_Transform_Draw(playerImage, *it);
		}

		for (list<SPRITE>::iterator it = hostileBullets.begin(); it != hostileBullets.end(); ++it)
		{
				Sprite_Transform_Draw(bulletsImage, *it);
		}

		for (list<SPRITE>::iterator it = hostilePlanes.begin(); it != hostilePlanes.end(); ++it)
		{
				Sprite_Transform_Draw(hostileImage, *it);
		}

		for (list<SPRITE>::iterator it = booms.begin(); it != booms.end(); ++it)
		{
				Sprite_Transform_Draw(boomImage, *it);
		}
}

void HostBroadCast(DWORD now)
{
		// bullets

		// players

		// hostile

		// boom
}

void ClientPush(DWORD now)
{
		// self position

		// new bullets
}

bool Game_Init(HWND window)
{
		srand(time(NULL));

		//initialize Direct3D
		if (!Direct3D_Init(window, SCREENW, SCREENH, false))
		{
				MessageBox(window, "Error initializing Direct3D", APPTITLE.c_str(), 0);
				return false;
		}

		//initialize DirectInput
		if (!DirectInput_Init(window))
		{
				MessageBox(window, "Error initializing DirectInput", APPTITLE.c_str(), 0);
				return false;
		}

		//create pointer to the back buffer
		d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

		//create a font
		font = MakeFont("Arial", 24);

		//load the game image as texture
		playerImage = LoadTexture("player.png", D3DCOLOR_XRGB(0, 0, 255));
		if (!playerImage)
		{
				MessageBox(window, "Error loading playerImage", APPTITLE.c_str(), 0);
				return false;
		}

		bulletsImage = LoadTexture("bullets.png", D3DCOLOR_XRGB(0, 0, 255));
		if (!bulletsImage)
		{
				MessageBox(window, "Error loading bulletsImage", APPTITLE.c_str(), 0);
				return false;
		}

		hostileImage = LoadTexture("hostile.png", D3DCOLOR_XRGB(0, 0, 255));
		if (!hostileImage)
		{
				MessageBox(window, "Error loading hostileImage", APPTITLE.c_str(), 0);
				return false;
		}

		boomImage = LoadTexture("boom.png", D3DCOLOR_XRGB(0, 0, 255));
		if (!boomImage)
		{
				MessageBox(window, "Error loading boomImage", APPTITLE.c_str(), 0);
				return false;
		}

		// init sprites
		player.scaling = 1.2;
		player.width = 32;
		player.height = 32;
		player.x = SCREENW / 2 - player.width * player.scaling / 2;
		player.y = SCREENH - player.height * player.scaling * 2;
		player.frame = 0;
		player.columns = 15;
		player.startframe = 0;
		player.endframe = 2;
		player.starttime = GetTickCount();
		player.delay = slowAnimationDelay;
		playerVisible = true;
		playerLife = 1;

		// init hostile
		hostileTimer = GetTickCount();
		hostileUpgradeTimer = GetTickCount();

		return true;
}

void Game_Run(HWND window)
{
		// now
		DWORD now = GetTickCount();

		if (!d3ddev) return;
		// input
		DirectInput_Update();
		// move control

		// clear all 
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 67, 171), 1.0f, 0);
		//keep the game running at a steady frame rate

		dt = now - screenTimer;
		if (dt >= msPerFrame)
		{
				//reset timing
				screenTimer = now;
				// boom check
				BOOMCheck(now);
				// move all sprites
				MoveSprites(now, dt);
				// moving
				if (playerVisible)
				{
						float newVelvx = 0;
						float newVelvy = 0;
						if (Key_Down(DIK_UP)) newVelvy += -1;
						if (Key_Down(DIK_DOWN)) newVelvy += 1;
						if (Key_Down(DIK_LEFT)) newVelvx += -1;
						if (Key_Down(DIK_RIGHT)) newVelvx += 1;

						float unify = 1;
						if (newVelvx != 0 || newVelvy != 0)
								unify = sqrt(newVelvx * newVelvx + newVelvy * newVelvy);
						player.velx = playerPixPerSecond * newVelvx / unify;
						player.vely = playerPixPerSecond * newVelvy / unify;
				}
				// shooting
				if (playerVisible && Key_Down(DIK_SPACE) && (now - shootTimer >= playerShootingInterval))
				{
						shootTimer = now;
						SPRITE newbullet;
						newbullet.width = newbullet.height = 9;
						newbullet.x = player.x + player.width  * player.scaling / 2 - newbullet.width * newbullet.scaling / 2;
						newbullet.y = player.y - newbullet.height * newbullet.scaling;
						newbullet.frame = 0;
						newbullet.columns = 2;
						newbullet.startframe = 0;
						newbullet.endframe = 0;
						newbullet.velx = 0;
						newbullet.vely = -friendlyBulletPixPerSecond;
						friendlyBullets.push_back(newbullet);
				}
				// update from others

				// bounds check
				BoundsCheck();
				// create hostile
				HostileUpgrade(now);
				HostileIncoming(now);

				// start broadcast


				//start rendering
				if (d3ddev->BeginScene())
				{

						spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
						/*
						std::ostringstream oss;
						oss << "Score = " << score;
						FontPrint(font, 5, 0, oss.str());
						*/

						DrawSprites(now);

						spriteobj->End();

						//stop rendering
						d3ddev->EndScene();
						d3ddev->Present(NULL, NULL, NULL, NULL);
				}
		}

		//if (playerLife <= 0 && friendlyPlanes.size() == 0)
				//gameover = true;

		//to exit 
		if (KEY_DOWN(VK_ESCAPE))
				gameover = true;
}

void Game_End()
{
		if (gameWorld) gameWorld->Release();
		if (playerImage) playerImage->Release();
		if (font) font->Release();
		DirectInput_Shutdown();
		Direct3D_Shutdown();
}
