// Player.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FFPlayer.hpp"

FFPlayer *player;

int main()
{
	player = new FFPlayer();
	player->Open("1.mp4");
	player->Play();

	while (true)
	{

	}

	return 0;
}

