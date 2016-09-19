// Player.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FFPlayer.hpp"

FFPlayer *player;

int _tmain(int argc, _TCHAR* argv[])
{
	player = new FFPlayer();
	player->Open("rtsp://184.72.239.149/vod/mp4:BigBuckBunny_115k.mov");
	//player->Open("1.mp4");
	player->Play();

	while (true)
	{

	}

	return 0;
}

