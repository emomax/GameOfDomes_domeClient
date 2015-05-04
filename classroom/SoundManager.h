#pragma once 
#include "sgct.h"

#include <al.h>
#include <alc.h>
#include <iostream>
#include <string.h>


class SoundManager {

public:
	SoundManager();
	virtual ~SoundManager();
	void init();

	void play(std::string score, glm::vec3 position);
	void stopMusic();
	void pauseMusic(){}

private:
	// SFX
	ALuint explosionSource;
	ALuint laserSource;

	// SCORES
	ALuint inGameSource;
	ALuint preGameSource;
	ALuint menuMusicSource;
	ALuint gameMusicSource;
	ALuint gameOverSource;

	// To keep track on what music is currently playing
	ALuint *currentBackgroundScore;
	bool bgIsPlaying = false;

	ALfloat SourcePos[3];
	ALfloat SourceVel[3];
	ALfloat ListenerPos[3];
	ALfloat ListenerVel[3];
	ALfloat ListenerOri[6];


	void setSource(ALuint *source, char *url);
	ALenum setSoundFormat(short bitsPerSample);
};