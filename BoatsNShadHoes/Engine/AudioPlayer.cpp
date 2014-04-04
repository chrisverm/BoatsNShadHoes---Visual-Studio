#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(const char* filename)
{
	// defaults
	manager = nullptr;
	doesLoop = AL_FALSE;

	for(int i=0; i < sizeof(position)/sizeof(float); i++)
		position[i] = 0.0f;

	for(int j=0; j < sizeof(velocity)/sizeof(float); j++)
		velocity[j] = 0.0f;

	gain = 1.0f;
	pitch = 1.0f;

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

float* const AudioPlayer::Position()
{
	return &position[0];
}

float* const AudioPlayer::Velocity()
{
	return &position[0];
}

void AudioPlayer::load(const char* filename)
{
	manager = new AudioManager(filename);

	// set player data based on the file that was loaded
	size = ALsizei(manager->dataSize());
	frequency = ALsizei(manager->Frequency());
	format = ALsizei(manager->format());
	data = manager->audioData();
}

void AudioPlayer::init()
{
	alGenSources(1, &source);
	alGenBuffers(1, &abo);

	generateBufferData();
}

void AudioPlayer::generateBufferData()
{
	// passes in the audio data to the specified buffer
	alBufferData(abo, format, (void*)data, size, frequency);

	/* Define source settings */
	alSourcei(source, AL_BUFFER, abo);				// attach buffer
	alSourcei(source, AL_LOOPING, doesLoop);		// isLooping
	alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE); // source is relative to listener
	alSourcef(source, AL_PITCH, pitch);				// pitch - default at 1.0f
	alSourcef(source, AL_GAIN, gain);				// gain - default at 1.0f
	alSourcefv(source, AL_POSITION, position);		// position
	alSourcefv(source, AL_VELOCITY, velocity);		// velocity

	// store the current state of the source
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::update()
{
	updatePosition();

	// pitch, gain
	alSourcef(source, AL_PITCH, pitch);
	alSourcef(source, AL_GAIN, gain);

	// position, velocity
	alSourcefv(source, AL_POSITION, position);
	alSourcefv(source, AL_VELOCITY, velocity);
	
	// looping
	alSourcei(source, AL_LOOPING, doesLoop);

	// state
	alGetSourcei(source, AL_SOURCE_STATE, &state);
}

void AudioPlayer::updateGain(float lPos[])
{
	// find distance between source and listener
	// distance(source (S), listener (L)) = sqrt ((Lx - Sx)^2 + (Ly - Sy)^2 + (Lz - Sz)^2)
	float distance = std::sqrtf(std::powf((lPos[0] - position[0]), 2) + 
								std::powf((lPos[1] - position[1]), 2) +
								std::powf((lPos[2] - position[2]), 2));

	gain = 1.0f / distance;

	// limit
	if(gain > 1.5f)
		gain = 1.5f;

/*#ifdef DEBUG
	std::cout << "Distance between source and listener: " << distance << std::endl;
#endif*/
}

void AudioPlayer::changePitch(float amnt)
{
	pitch += amnt;

	// clamp at 2.0f
	if(pitch > 2.0f)
		pitch = 2.0f;

	// clamp at 0.1f (values of 0.0f or lower are invalid!)
	if(pitch <= 0.0f)
		pitch = 0.1f;
}

/*
 * Updates position of the audio based on position
 */
void AudioPlayer::updatePosition()
{
	position[0] += velocity[0];
	position[1] += velocity[1];
	position[2] += velocity[2];
}

void AudioPlayer::changeVelocity(float x, float y, float z)
{
	velocity[0] += x;
	velocity[1] += y;
	velocity[2] += z;
}

void AudioPlayer::setLooping(ALboolean setting)
{
	doesLoop = setting;
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