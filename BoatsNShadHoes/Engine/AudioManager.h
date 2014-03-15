#ifndef __AUDIOMANAGER_H
#define __AUDIOMANAGER_H

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "OpenAL\include\AL\al.h"
#include "OpenAL\include\AL\alc.h"

/*
 * Sources: 
 *	https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
 *	http://www.dunsanyinteractive.com/blogs/oliver/?p=72 
 *
 * WAVE PCM soundfile format
 *
 * Struct that holds the RIFF data of the WAVE file.
 * The RIFF data is the meta data information that holds
 * the ID, size, and format of the WAVE file
 *
 * "RIFF" chunk descriptor
 */
struct RIFF_Header
{
	char chunkID[4]; // contains the word "RIFF"
	uint32_t chunkSize; // size of the rest of the data
	char format[4]; // contains the word "WAVE"
};

/*
 * Struct to hold fmt sub-chunk for WAVE files.
 * Describes the format of the sound information in the data sub-chunk
 *
 * "fmt " sub-chunk
 */
struct WAVE_Format
{
	char subChunkID[4]; // contains the word "fmt "
	uint32_t subChunkSize; // 16 for PCM(pulse-code-modulation). The size of the rest of the subchunk which follows this number
	uint16_t audioFormat; // PCM = 1 (linear quantization) values other than 1 indicate compression
	uint16_t numChannels; // Mono = 1, Stereo = 2, etc.
	uint32_t sampleRate; // 8000, 44100, etc.
	uint32_t byteRate; // SampleRate * NumChannels * BitsPerSample/8
	uint16_t blockAlign; // NumChannels * BitsPerSample/8 : number of bytes for one sample including all channels
	uint16_t bitsPerSample; // 8 bits = 8, 16 bits = 16, etc.
};

/*
 * Struct to hold data sub-chunk for WAVE files
 * Indicates the size of the sound information and contains the raw sound data.
 *
 * "data" sub-chunk
 */
struct WAVE_Data
{
	char subChunkID[4]; // contains the word "data"
	uint32_t subChunk2Size; // Stores the size of the data block
};

class AudioManager
{
private:
	RIFF_Header* riff_header;
	WAVE_Format* wave_format;
	WAVE_Data* wave_data;

	unsigned char* data;

public:
	AudioManager();
	AudioManager(const char* filename);
	~AudioManager();

	/* Accessors */
	unsigned char* audioData() const;
	uint32_t dataSize() const;
	uint32_t frequency() const;
	uint16_t numChannels() const;
	uint16_t bitsPerSample() const;
	ALenum format() const;

	void loadWAV(const char* filename);
};

#endif