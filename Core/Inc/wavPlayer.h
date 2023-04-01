#include "main.h"
#include "stm32f0xx_hal.h"

#define AUDIO_BUFFER_SIZE		512
#define WAV_FILE_HEADER_SIZE	44

#define CHUNK_ID_CONST			0x46464952
#define FORMAT_CONST			0x45564157
#define CHANNEL_STEREO			2

struct WavFilesInfo
{
	uint32_t chunkId;
	uint32_t chunkSize;
	uint32_t format;
	uint32_t subchunk1Id;
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	uint32_t subchunk2Id;
	uint32_t subchunk2Size;
};

bool SDMount(void);
bool WAVPlayerFileSelect(const char* filePath);
void WAVPlayerPlay(I2S_HandleTypeDef* i2s);
bool WAVPlayerStopAndCloseFile(void);
void WAVPlayerFillBuffer(I2S_HandleTypeDef* i2s, bool bufferHalf);
