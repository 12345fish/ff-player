// ff-player.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ryulib/base.hpp>
#include "FFPlayer.h"
#include "AudioRenderer.h"
#include "VideoRenderer.h"

using namespace std;

FFPlayer *player;

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc == 1) {
		printf("C:\> ff-player.exe [Video File]");
		return 0;
	}

	player = new FFPlayer();

	player->setAudioRenderer(new AudioRenderer());
	player->setVideoRenderer(new VideoRenderer());

//	player.Open("rtsp://184.72.239.149/vod/mp4:BigBuckBunny_115k.mov");
	player->Open(argv[1]);
	player->Play();

	while (true) {
		printf("* q: Quit, p: Play, s: Stop \n");

		char key = getchar();

		switch (key) {
		case 'q': {
			player->Close();
			return 0;
		}

		case 'p': player->Play(); break;
		case 's': player->Stop(); break;
		}
	}

	player->Stop();

	return 0;
}

