/**
 * Pong.cpp -- Part of SDL_Pong
 * This is the implementation file for a simple
 * pong game using SDL
 *
 * Author: Dennis J. McWherter, Jr.
 * Date: 5 February 2011
 */

#include "Pong.h"

Pong::Pong(int x,int y)
	: _x(x), _y(y), _running(0), _paused(0), _pQueue(0), _delay(43), _pSpeed(15),
	_speed(10), _newdir(0), _xdir(0), _ydir(0), _moving(0)
{
	// Can't finish using init list because SDL needs to be initialized
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		throw "Could not initialize SDL!";

	// Start setting things up
	_gamescreen = SDL_SetVideoMode(_x,_y,8,SDL_SWSURFACE);

	// Player 1
	_p1.w = 10;
	_p1.h = 50;
	_p1.x = 5;
	_p1.y = (_y/2) - (_p1.h/2);

	// Player 2
	_p2.w = 10;
	_p2.h = 50;
	_p2.x = (_x - _p2.w)-5;
	_p2.y = (_y/2) - (_p2.h/2);

	// The ball - otherwise we just have two paddles o_O
	_ball.w = 9;
	_ball.h = 9;
	_ball.x = _x/2;
	_ball.y = _y/2;

	// Setup our colors - we're dull... Black & white...
	_black = SDL_MapRGB(_gamescreen->format,0x00,0x00,0x00);
	_white = SDL_MapRGB(_gamescreen->format,0xff,0xff,0xff);

	// Finish updating or initiale scene
	SDL_FillRect(_gamescreen,&_p1,_white);
	SDL_FillRect(_gamescreen,&_p2,_white);
	SDL_FillRect(_gamescreen,&_ball,_white);

	// Add items to our update queue
	_queue[_pQueue++] = _p1;
	_queue[_pQueue++] = _p2;
	_queue[_pQueue++] = _ball;

	// Window options
	SDL_WM_SetCaption("SDL_Pong - Dennis J. McWherter, Jr.",NULL);
	SDL_ShowCursor(SDL_DISABLE);
}

Pong::~Pong()
{
}

void Pong::run()
{
	_running = 1;
	int pressed = 0;
	SDLKey last = (SDLKey)0, last2 = (SDLKey)0; // Last is main & for p1 - last2 is for p2
	while(_running)
	{
		// (un)pause the game
		if(_paused != 0)
		{
			while(SDL_PollEvent(&_event))
			{
				if(_event.type == SDL_KEYDOWN)
					if(_event.key.keysym.sym == SDLK_p)
						_paused = 0;
				SDL_Delay(_delay);
			}
			continue;
		}

		// Do we have a queue? Update it if so
		if(_pQueue)
		{
			SDL_UpdateRects(_gamescreen,_pQueue,_queue);
			_pQueue = 0;
		}

		while(SDL_PollEvent(&_event))
		{
			// Handle events
			switch(_event.type)
			{
			case SDL_QUIT:
				_running = 0;
			break;
			case SDL_KEYDOWN:
			{
				switch(_event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					_running = 0;
				break;
				case SDLK_p:
					_paused = 1;
				break;
				case SDLK_s: // Start/stop
					if(_moving == 0)
						moveBall();
					else
						resetBall();
				break;
				case SDLK_UP:
					pressed = 1;
					last = SDLK_UP;
					movePaddle(0,1);
				break;
				case SDLK_DOWN:
					pressed = 1;
					last = SDLK_DOWN;
					movePaddle(0,0);
				break;
				case SDLK_PAGEUP:
					pressed = 1;
					last2 = SDLK_PAGEUP;
					movePaddle(1,1);
				break;
				case SDLK_PAGEDOWN:
					pressed = 1;
					last2 = SDLK_PAGEDOWN;
					movePaddle(1,0);
				break;
				default:
				break;
				}
			}
			break;
			case SDL_KEYUP:
			{
				switch(_event.key.keysym.sym)
				{
				case SDLK_DOWN:
				case SDLK_UP:
					last = (SDLKey)NULL;
				break;
				case SDLK_PAGEUP:
				case SDLK_PAGEDOWN:
					last2 = (SDLKey)NULL;
				break;
				default:
				break;
				}
			if(last == 0 && last2 == 0)
				pressed = 0;
			}
			break;
			default:
				// Do nothing
			break;
			}
		}
		if(pressed > 0)
		{
			switch(last) // Player 1
			{
			case SDLK_DOWN:
				movePaddle(0,0);
			break;
			case SDLK_UP:
				movePaddle(0,1);
			break;
			default:
			break;
			}

			switch(last2) // Player 2
			{
			case SDLK_PAGEUP:
				movePaddle(1,1);
			break;
			case SDLK_PAGEDOWN:
				movePaddle(1,0);
			break;
			default:
			break;
			}

		}
		if(_moving != 0)
			moveBall();
		else
			resetBall();
		SDL_Delay(_delay); // Save the CPU so your PC doesn't blow up playing Pong :)
	}
	SDL_Quit();
}

// Which direction did we move? 0 = down, 1 = up - also, 0 = p1 and 1 = p2
void Pong::movePaddle(int p,int dir)
{
	SDL_Rect &paddle = (p > 0) ? _p2 : _p1; // Which paddle are we moving?
	int y = (dir > 0) ? (paddle.y - _pSpeed) : (paddle.y + _pSpeed);

	if(y < 0 || y+paddle.h > _y) return; // Don't go off the screen

	SDL_FillRect(_gamescreen,&paddle,_black);
	_queue[_pQueue++] = paddle;

	paddle.y = y;
	SDL_FillRect(_gamescreen,&paddle,_white);
	_queue[_pQueue++] = paddle;
}

// Move the ball - d = direction, -1 = left, 1 = right, 0 = random (start of game)
void Pong::moveBall()
{
	srand((unsigned int)time(0));
	int v[3] = {0,1,-1}; // Our valid value set
	if(_moving == 0)
	{
		_xdir = v[(rand() % 2 + 1)];
		_ydir = v[(rand() % 2)];
	}
	int x = _ball.x+(_xdir*_speed);
	int y = _ball.y+(_ydir*(_speed)); // Guess where the ball is going? :)

	_moving = 1;

	// Check our collision points
	checkCollision(x,y);

	// Remove old position
	SDL_FillRect(_gamescreen,&_ball,_black);
	_queue[_pQueue++] = _ball;

	// Update ball's position
	_ball.x = x;
	_ball.y = y;
	SDL_FillRect(_gamescreen,&_ball,_white);
	_queue[_pQueue++] = _ball;
}

void Pong::resetBall()
{
	SDL_FillRect(_gamescreen,&_ball,_black);
	_queue[_pQueue++] = _ball;
	_ball.x = _x/2;
	_ball.y = _y/2;
	SDL_FillRect(_gamescreen,&_ball,_white);
	_queue[_pQueue++] = _ball;
}

void Pong::checkCollision(const int & x,int & y)
{
	srand((unsigned int)time(0));
	// Calculate paddle displacement
	int dx  = (_xdir < 0) ? (_p1.w+_ball.w+5) : ((_p2.x-_p2.w)-_ball.w);
	int dyt = (_xdir < 0) ? (_p1.y) : (_p2.y);
	int dyb = (_xdir < 0) ? (_p1.y+_p1.h) : (_p2.y+_p2.h);
	int n = rand() % 2;

	if(((_xdir < 0) ? x <= dx : x >= dx) && (y >= dyt && y <= dyb)) // Hit!
	{
		_newdir = 1;
		_xdir *= -1;
		_ydir = (_ydir == 0) ? rand() % 5 : ((_ydir/_ydir)*((rand() % 4)-2));
	}
	
	if(y <= 0)
	{
		y = 2; // Give it an offset so we don't fall off the screen xD
		_ydir = (n == 0) ? 1 : n;
	} else if(y >= (_y-_ball.h)) {
		y = _y-2-_ball.h;
		_ydir = -1*((n == 0) ? 1 : n);
	}

	if(x < _ball.w || x > (_p2.x-_ball.w)) // Score!
		_moving = 0;
}
