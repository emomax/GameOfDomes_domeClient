#include "SoundManager.h"

//ALC specifics
ALCcontext *context;
ALCdevice *device;

// File reading stuff 

FILE *fp = NULL;

char type[4];
DWORD size, chunkSize;
short formatType, channels;
DWORD sampleRate, avgBytesPerSec;
short bytesPerSample, bitsPerSample;
DWORD dataSize;

// Spatial sound attributes

SoundManager::SoundManager() {
	std::cout << "SoundManager is alive!" << std::endl;
	
	SourcePos[0] = 0.0f; SourcePos[1] = 0.0f; SourcePos[2] = 0.0f;
	SourceVel[0] = 0.0f; SourceVel[1] = 0.0f; SourceVel[2] = 0.0f;
	ListenerPos[0] = 0.0f; ListenerPos[1] = 0.0f; ListenerPos[2] = 0.0f;
	ListenerVel[0] = 0.0f; ListenerVel[1] = 0.0f; ListenerVel[2] = 0.0f;
	
	ListenerOri[0] = 0.0f; ListenerOri[1] = 0.0f; ListenerOri[2] = 0.0f;
	ListenerOri[3] = 0.0f; ListenerOri[4] = 0.0f; ListenerOri[5] = 0.0f;


}

SoundManager::~SoundManager() {

}

void SoundManager::init() {
	std::cout << "SoundManager initiated!" << std::endl;

	// Init OpenAL
	device = alcOpenDevice(NULL);
	if (!device) return;
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	if (!context) return;



	/*alGenSources(1, &gameMusicSource);*/
	alGenSources(1, &explosionSource);
	alGenSources(1, &menuMusicSource);
	alGenSources(1, &laserSource);

	/*setSource(&gameMusicSource, "sounds/gameOver2.wav");*/
	setSource(&explosionSource, "sounds/explosion1.wav");
	setSource(&menuMusicSource, "sounds/gameOver2.wav");
	setSource(&laserSource, "sounds/laser.wav");


}

//! Plays the score parameter at given position vec3.
void SoundManager::play(std::string score, glm::vec3 position) {

	if (score == "gameOver") {
		alSourcePlay(menuMusicSource);
	}
	if (score == "laser") {
		alSourcePlay(laserSource);
	}
	if (score == "explosion") {
		alSourcePlay(explosionSource);
	}
}

//! Takes parameter URL, loads file and sets up sound for source parameter
void SoundManager::setSource(ALuint *source, char *url) {
	std::cout << "Attempting to set '" << url << "' as source for sound!" << std::endl;

	// Load the current url
	fp = fopen(url, "rb");

	// Check riff type, wave type and fmt
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F') return;

	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E') return;

	fread(type, sizeof(char), 4, fp); 
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ') return;

	// read additional info about file.

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	// make sure file has been read
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'd' || type[1] != 'a' || type[2] != 't' || type[3] != 'a') return;
	fread(&dataSize, sizeof(DWORD), 1, fp);

	// allocate memory for sound data and read the data
	unsigned char* buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);

	// Sound generation

	ALuint buffer;
	ALuint frequency = sampleRate;
	ALenum format = 0;

	alGenBuffers(1, &buffer);
	alGenSources(1, source);

	// let OpenAL know the format of the sound
	format = setSoundFormat(bitsPerSample);

	alBufferData(buffer, format, buf, dataSize, frequency);

	// Listener
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);

	alSourcei(*source, AL_BUFFER, buffer);
	alSourcef(*source, AL_PITCH, 1.0f);
	alSourcef(*source, AL_GAIN, 1.0f);
	alSourcefv(*source, AL_POSITION, SourcePos);
	alSourcefv(*source, AL_VELOCITY, SourceVel);
	alSourcei(*source, AL_LOOPING, AL_FALSE);
}

ALenum SoundManager::setSoundFormat(short bitsPerSample) {
	if (bitsPerSample == 8) {
		if (channels == 1)
			return AL_FORMAT_MONO8;
		else if (channels == 2)
			return AL_FORMAT_STEREO8;
	}
	else if (bitsPerSample == 16) {
		if (channels == 1)
			return AL_FORMAT_MONO16;
		else if (channels == 2)
			return AL_FORMAT_STEREO16;
	}
}