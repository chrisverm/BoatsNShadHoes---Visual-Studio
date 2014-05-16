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

	float position[3];
	float velocity[3];
	float gain;
	float pitch;

	char* data;

	void load(const char*);

public:
	AudioPlayer(const char*);
	~AudioPlayer();

	/* Accessors */
	ALuint State() const;
	float* const Position();
	float* const Velocity();

	/* Mutators */
	void setLooping(ALboolean);
	
	void init();
	void generateBufferData();
	void update();
	void updateGain(float[]);

	void changePitch(float);
	void updatePosition();
	void changeVelocity(float, float=0.0f, float=0.0f);
	void setPosition(float, float, float);

	// Control playing
	void pause();
	void play();
	void rewind();
	void start();
	void stop();
	bool playing();
};

#endif