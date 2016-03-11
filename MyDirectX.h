/*  Beginning Game Programming
	MyDirectX.h
*/

#pragma once

//header files
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <xinput.h>
#include <ctime>
#include <iostream>
#include <iomanip>

using namespace std;

//libraries
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"xinput.lib")

//program values
extern const string APPTITLE;
extern const int SCREENW;
extern const int SCREENH;
extern bool gameover;
extern bool gamestarted;
extern int id;

//macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

//Direct3D objects
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DSURFACE9 backbuffer;
extern LPD3DXSPRITE spriteobj;

//sprite structure
struct SPRITE
{
		int id;
		float x, y;
		float velx, vely;
		int frame, columns;
		int width, height;
		float scaling, rotation;
		int startframe, endframe;
		DWORD starttime, delay;
		int direction;
		D3DCOLOR color;

		SPRITE()
		{
				id = 0;
				x = y = 0;
				frame = 0;
				columns = 1;
				width = height = 0;
				scaling = 1.0f;
				rotation = 0.0f;
				startframe = endframe = 0;
				direction = 1;
				starttime = delay = 0;
				velx = vely = 0.0f;
				color = D3DCOLOR_XRGB(255, 255, 255);
		}
};

//Direct3D functions
bool Direct3D_Init(HWND hwnd, int width, int height, bool fullscreen);
void Direct3D_Shutdown();
LPDIRECT3DSURFACE9 LoadSurface(string filename);
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source);
LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor = D3DCOLOR_XRGB(0, 0, 0));
void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 texture, int destx, int desty,
		int framenum, int framew, int frameh, int columns);
void Sprite_Animate(int &frame, int startframe, int endframe, int direction, DWORD &starttime, int delay, DWORD now, bool looping = true);
//*** ADDED in chapter 7
void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, int x, int y, int width, int height,
		int frame = 0, int columns = 1, float rotation = 0.0f, float scaling = 1.0f,
		D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

void Sprite_Transform_Draw(LPDIRECT3DTEXTURE9 image, SPRITE sprite, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));

//bounding box collision detection
int Collision(SPRITE sprite1, SPRITE sprite2);

//distance based collision detection
bool CollisionD(SPRITE sprite1, SPRITE sprite2);

//DirectInput objects, devices, and states
extern LPDIRECTINPUT8 dinput;
extern LPDIRECTINPUTDEVICE8 dimouse;
extern LPDIRECTINPUTDEVICE8 dikeyboard;
extern DIMOUSESTATE mouse_state;
extern XINPUT_GAMEPAD controllers[4];

//DirectInput functions
bool DirectInput_Init(HWND);
void DirectInput_Update();
void DirectInput_Shutdown();
bool Key_Down(int);
int Mouse_Button(int);
int Mouse_X();
int Mouse_Y();
void XInput_Vibrate(int contNum = 0, int amount = 65535);
bool XInput_Controller_Found();

//game functions
bool Game_Init(HWND window);
void Game_Option(HWND window);
void Game_Run(HWND window);
void Game_Exit();
void Game_End();
void Game_Start();

//font functions
LPD3DXFONT MakeFont(string name, int size);
void FontPrint(LPD3DXFONT font, int x, int y, string text, D3DCOLOR color = D3DCOLOR_XRGB(255, 255, 255));
