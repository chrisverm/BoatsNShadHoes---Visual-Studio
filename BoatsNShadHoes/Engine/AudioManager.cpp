#include "AudioManager.h"

/*
 * Default constructor
 *
 * Sets up default values.
 * Does not load the audio file.
 */
AudioManager::AudioManager()
{
	data = nullptr;
	vectorData = std::vector<char>();
}

/*
 * Parameterized constructor
 * Sets up default values.
 * Loads the WAVE file
 */
AudioManager::AudioManager(std::string filepath)
{
	data = nullptr;
	vectorData = std::vector<char>();

	// check file extension
	unsigned short last = filepath.find_last_of(".");

	// load audio based on extension
	if(filepath.substr(last+1) == "wav")
	{
		std::cout << "A WAV" << std::endl;
		loadWAV(filepath);
	}
	else if(filepath.substr(last+1) == "ogg")
	{
		std::cout << "OGG" << std::endl;
		loadOGG(filepath);
	}
	else
		std::cout << "Unsupported file type!" << std::endl;
}

/*
 * Clear up any necessary memory
 */
AudioManager::~AudioManager()
{
	if(data != nullptr)
		delete[] data;
}

/*
 * Returns our sound data (in bytes).
 */
char* AudioManager::audioData()
{
	if(data == nullptr)
	{
		data = &vectorData[0];
		return data;
	}

	else
		return data;
}

/*
 * Returns the size of the audio data.
 */
uint32_t AudioManager::dataSize() const
{
	//return wave_data->subChunk2Size;

	//return (uint32_t)vectorData.size();

	return size;
}

/*
 * Returns the sample rate of the audio.
 */
uint32_t AudioManager::Frequency() const
{
	//return wave_format->sampleRate;

	return audioFrequency;
}

/*
 * Returns the number of channels of the audio.
 */
/*uint16_t AudioManager::numChannels() const
{
	return wave_format->numChannels;
}*/

/*
 * Returns the bits per sample of the audio
 */
/*uint16_t AudioManager::bitsPerSample() const
{
	return wave_format->bitsPerSample;
}*/

/*
 * Returns the format of the audio
 * Available formats:
 *	MONO8: 1 Channel / 8 Bits Per Channel
 *	MONO16: 1 Channel / 16 Bits Per Channel
 *	STEREO8: 2 Channels / 8 Bits Per Channel
 *	STEREO16: 2 Channels / 16 Bits Per Channel
 */
ALenum AudioManager::format() const
{
	/*if(wave_format->numChannels == 1)
	{
		if(wave_format->bitsPerSample == 8)
			return AL_FORMAT_MONO8;

		else if(wave_format->bitsPerSample == 16)
			return AL_FORMAT_MONO16;
	}

	else if(wave_format->numChannels == 2)
	{
		if(wave_format->bitsPerSample == 8)
			return AL_FORMAT_STEREO8;

		else if(wave_format->bitsPerSample == 16)
			return AL_FORMAT_STEREO16;
	}*/

	return audioFormat;
}

void AudioManager::loadOGG(std::string filepath)
{
	FILE* soundFile = NULL;

	vorbis_info* pInfo;
	OggVorbis_File oggFile;

	try
	{
		fopen_s(&soundFile, filepath.c_str(), "rb");

		// open file using ogg vorbis
		ov_open(soundFile, &oggFile, NULL, 0);

		// grab information about the file
		pInfo = ov_info(&oggFile, -1);

		// format (always use 16-bit samples)
		if(pInfo->channels == 1)
			audioFormat = AL_FORMAT_MONO16;
		else
			audioFormat = AL_FORMAT_STEREO16;

		// frequency (of sampling rate)
		audioFrequency = pInfo->rate;

		// decode ogg file
		long bytes;
		char array[AUDIO_BUFFER_SIZE];
		int bitStream;

		//data = new char[0];
		//long currentSize = 0;

		//vectorData = std::vector<char>();

		do{
			// read up to a buffer's worth of decoded sound data
			bytes = ov_read(&oggFile, array, AUDIO_BUFFER_SIZE, 0, 2, 1, &bitStream);

			// increase size of buffer
			//data = (char*) realloc(data, currentSize + AUDIO_BUFFER_SIZE * sizeof(char));

			//currentSize += AUDIO_BUFFER_SIZE;

			// append to end of buffer
			vectorData.insert(vectorData.end(), array, array + bytes);
			//strcat(data, array);
			//strcat_s(data, currentSize, array);

		}while (bytes>0); // while not End of File

		//vectorData.insert(vectorData.end(), '\0');

		// set size
		size = (uint32_t)vectorData.size();
		//size = sizeof(data)/sizeof(char);

		data = new char[size];

		time_t timer;
		
		timer = time(NULL);

		// copy data over! :D
		for(int i=0; i < vectorData.size(); i++)
			data[i] = vectorData[i];

		//std::copy(vectorData.begin(), vectorData.end(), data);

		std::cout << difftime(timer, time(NULL)) << std::endl;

		// clean up memory
		ov_clear(&oggFile);
	}

	catch(char* error)
	{
		ov_clear(&oggFile);
	}
}

/* 
 * Loads the WAVE file and stores all of the information for it
 * in our variables
 */
void AudioManager::loadWAV(std::string filename)
{
	FILE* soundFile = NULL;
	RIFF_Header* riff_header = new RIFF_Header();
	WAVE_Format* wave_format = new WAVE_Format();
	WAVE_Data* wave_data = new WAVE_Data();

	try
	{
		// safely opens the file using read-only binary mode
		fopen_s(&soundFile, filename.c_str(), "rb");

		/* 
		 * read in the first chunk of memory into our struct
		 *
		 * This will contain the following:
		 * ChunkID - "RIFF"
		 * ChunkSize - The size of the rest of the chunk following this number
		 * Format - The format of the file. In our case, it should be "WAVE"
		 */
		fread(riff_header, sizeof(RIFF_Header), 1, soundFile);

		// check for RIFF and WAVE tag in memory
		// Exits if it did not work!
		if((riff_header->chunkID[0] != 'R' ||
			riff_header->chunkID[1] != 'I' ||
			riff_header->chunkID[2] != 'F' ||
			riff_header->chunkID[3] != 'F') ||
		   (riff_header->format[0] != 'W' ||
			riff_header->format[1] != 'A' ||
			riff_header->format[2] != 'V' ||
			riff_header->format[3] != 'E'))
				throw("Invalid RIFF or WAVE header!");

		//std::cout << riff_header->chunkID[0] << riff_header->chunkID[1] << riff_header->chunkID[2] << riff_header->chunkID[3] << std::endl;
		//std:: cout << riff_header->format[0] << riff_header->format[1] << riff_header->format[2] << riff_header->format[3] << std::endl;
	
		/*
		 * read in the second chunk of memory related to the WAVE format
		 *
		 * This will contain the following:
		 * Subchunk1ID - "fmt "; Note the space after fmt
		 * Subchunk1Size - 16 for PCM (Pulse-code modulation : method of encoding audio information digitally)
		 * AudioFormat - PCM = 1 (linear quantization) values other than 1 indicate some form of compression
		 * NumChannels - Mono = 1, Stereo = 2, etc.
		 * SampleRate - 8000, 44100, etc.
		 * ByteRate - SampleRate * NumChannels * BitsPerSample/8
		 * BlockAlign - NumChannels * BitsPerSample/8
		 * BitsPerSample - 8 bits = 8, 16 bits = 16, etc.
		 */
		fread(wave_format, sizeof(WAVE_Format), 1, soundFile);

		// check for "fmt " tag in memory
		if(wave_format->subChunkID[0] != 'f' ||
		   wave_format->subChunkID[1] != 'm' ||
		   wave_format->subChunkID[2] != 't' ||
		   wave_format->subChunkID[3] != ' ')
				throw("Invalid WAVE format");

		// check for extra parameters
		if(wave_format->subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		// set frequency
		audioFrequency = wave_format->sampleRate;

		// set format
		if(wave_format->numChannels == 1)
		{
			if(wave_format->bitsPerSample == 8)
				audioFormat = AL_FORMAT_MONO8;

			else if(wave_format->bitsPerSample == 16)
				audioFormat = AL_FORMAT_MONO16;
		}

		else if(wave_format->numChannels == 2)
		{
			if(wave_format->bitsPerSample == 8)
				audioFormat = AL_FORMAT_STEREO8;

			else if(wave_format->bitsPerSample == 16)
				audioFormat =  AL_FORMAT_STEREO16;
		}

		else
			audioFormat = AL_INVALID_ENUM;

		if(AL_INVALID_ENUM == audioFormat)
			throw("Invalid audio format!");

		/* 
		 * read in the last bit of information before the actual raw sound data 
		 * 
		 * This will contain the following:
		 * Subchunk2ID - "data"
		 * Subchunk2Size - NumSamples * NumChannels * BitsPerSample/8 (the number of bytes in the data - 
		 * the size of the read of the subchunk following this number)
		 */
		fread(wave_data, sizeof(WAVE_Data), 1, soundFile);

		// check for "data" tag in memory
		if(wave_data->subChunkID[0] != 'd' ||
		   wave_data->subChunkID[1] != 'a' ||
		   wave_data->subChunkID[2] != 't' ||
		   wave_data->subChunkID[3] != 'a')
				throw("Invalid data header!");

		size = wave_data->subChunk2Size;
		data = new char[wave_data->subChunk2Size];

		// read in the raw sound data
		if(!fread(data, wave_data->subChunk2Size, 1, soundFile))
			throw("error loading WAVE data!");

		// clean up memory
		fclose(soundFile);

		delete riff_header;
		delete wave_format;
		delete wave_data;
	}

	// catch any errors we throw
	catch(char* error)
	{
		std::cerr << error << " trying to load " << filename << std::endl;

		// clean up memory if necessary
		//if(soundFile != NULL)
			fclose(soundFile);

		delete riff_header;
		delete wave_format;
		delete wave_data;
	}
}