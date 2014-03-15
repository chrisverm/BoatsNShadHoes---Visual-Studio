#ifndef __AUDIOPLAYER_H
#define __AUDIOPLAYER_H

#include "AudioManager.h"

class AudioPlayer
{
private:
	AudioManager* manager;
	ALuint abo;
	ALuint source;

	ALsizei size;
	ALsizei frequency;
	ALint state;
	ALenum format;
	ALboolean doesLoop;

	unsigned char* data;

	void load(const char* filename);

public:
	AudioPlayer(const char* filename);
	~AudioPlayer();

	/* Accessors */
	ALuint State() const;

	/* Mutators */
	void setLooping(ALboolean setting);
	
	void init();
	void generateBufferData();

	// Control playing
	void pause();
	void play();
	void rewind();
	void start();
	void stop();
};

#endif