/**
 * main.cpp -- Part of SDL_Pong
 * This is the main of the pong game
 *
 * Author: Dennis J. McWherter, Jr.
 * Date: 2 February 2011
 */

#include "Pong.h"

int main(int argc,char **argv)
{
	try
	{
		Pong pong(640,480);
		pong.run();
	} catch(std::exception & e) {
		std::cerr << "Exception: "<< e.what() << std::endl;
	}
	return 0;
}
