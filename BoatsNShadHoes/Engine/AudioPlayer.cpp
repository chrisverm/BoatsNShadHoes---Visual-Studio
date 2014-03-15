#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(const char* filename)
{
	manager = nullptr;
	doesLoop = AL_FALSE;

	// begin loading
	load(filename);
}

AudioPlayer::~AudioPlayer()
{
	alDeleteBuffers(1, &abo);
	alDeleteSources(1, &source);

	if(manager != nullptr)
	{
		delete manager;
		manager = nullptr;
	}
}

ALuint AudioPlayer::State() const
{
	return state;
}

void AudioPlayer::load(const char* filename)
{
	manager = new AudioManager(filename);

	// set player data based on the file that was loaded
	size = ALsizei(manager->dataSize());
	frequency = ALsizei(manager->frequency());
	format = ALsizei(manager->format());
	data = manager->audioData();
}

void AudioPlayer::init()
{
	alGenSources(1, &source);
	alGenBuffers(1, &abo);
}

void AudioPlayer::generateBufferData()
{
	// passes in the audio data to the specified buffer
	alBufferData(abo, format, (void*)data, size, frequency);

	// source recieves the buffer
	alSourcei(source, AL_BUFFER, abo);

	// does not loop by default
	alSourcei(source, AL_LOOPING, doesLoop);

	// store the current state of the source
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::setLooping(ALboolean setting)
{
	doesLoop = setting;

	alSourcei(source, AL_LOOPING, doesLoop);
}

void AudioPlayer::pause()
{
	alSourcePause(source);
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::play()
{
	alSourcePlay(source);
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::rewind()
{
	alSourceRewind(source);
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::start()
{
	alSourcePlay(source);
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::stop()
{
	alSourceStop(source);
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}