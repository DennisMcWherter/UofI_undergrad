/**
 * Pong.h -- Part of SDL_Pong
 * This is the interface file for a simple
 * pong game using SDL
 *
 * Author: Dennis J. McWherter, Jr.
 * Date: 5 February 2011
 */

#ifndef __PONG_H__
#define __PONG_H__

#include <iostream>
#include <time.h>
#include "SDL/SDL.h"

#ifdef _WIN32
#pragma comment(lib,"SDL.lib")
#pragma comment(lib,"SDLmain.lib")
#endif

class Pong
{
public:
	Pong(int x,int y);
	~Pong();

	void run();
	void movePaddle(int p,int dir);
	void moveBall();
	void resetBall();
	void checkCollision(const int & x,int & y);

private:
	int _x,_y,_running,_paused,_pQueue,_delay,_pSpeed,_speed,_newdir,_xdir,_ydir,_moving;
	SDL_Surface *_gamescreen;
	SDL_Rect _p1,_p2,_ball,_queue[16];
	Uint32 _black,_white;
	SDL_Event _event;
};
#endif
