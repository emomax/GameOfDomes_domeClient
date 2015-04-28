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
	void stopMusic(){}
	void pauseMusic(){}

private:
	ALuint explosionSource;
	ALuint laserSource;
	ALuint menuMusicSource;
	ALuint gameMusicSource;


	ALfloat SourcePos[3];
	ALfloat SourceVel[3];
	ALfloat ListenerPos[3];
	ALfloat ListenerVel[3];
	ALfloat ListenerOri[6];


	void setSource(ALuint *source, char *url);
	ALenum setSoundFormat(short bitsPerSample);
};