#pragma once 
#include "sgct.h"

#include "Includes.h"

#include <al.h>
#include <alc.h>
#include <iostream>
#include <string.h>


class SoundManager {

public:
	SoundManager();
	virtual ~SoundManager();
	void init(float _bgVolume, float _soundVolume);

	void play(std::string score, osg::Vec3f position);
	void stopMusic();
	void pauseMusic(){}

	void muteAll();

private:
	// SFX
	ALuint explosionSource;
	ALuint laserSource;
	ALuint laserHitSource;
	ALuint powerupSource;

	// SCORES
	ALuint inGameSource;
	ALuint inGameLowHPSource;
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

	// GLOBALS
	bool _MUTED = false;
	float _bgVolume = 1.0f;
	float _soundVolume = 1.0f;

	void setSource(ALuint *source, char *url);
	ALenum setSoundFormat(short bitsPerSample);
};