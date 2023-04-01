/*
 * wav_player.c
 *
 * Created on: 30 Mar 2023
 *     Author: Mat L
 */

#include <wavPlayer.h>
#include "fatfs.h"

FATFS fatFs;			// file system
FIL wavFile;			// file
FIL file;
FRESULT fresult;		// to store the result
//char buffer[1024];		// to store data

UINT br, bw;

/* capacity related variables */
FATFS *pfatFs;
DWORD fre_clust;
uint32_t total, free_space;

UINT* nullptr = NULL;

static int16_t audioBuffer[AUDIO_BUFFER_SIZE];

struct WavFilesInfo wavFileInfo;

bool SDMount(void)
{
  /* Mount SD Card */
  if (f_mount(&fatFs, "", 0) == FR_OK)
  {
	  SendStringUART("SD card mounted succesful\r\n");
	  return true;
  }
  else
  {
	  SendStringUART("SD card mount failed\r\n");
	  return false;
  }
}

bool WAVPlayerFileSelect(const char* filePath)
{
	UINT readBytes = 0;
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

	/* Open WAV file to read */
	if (f_open(&wavFile, filePath, FA_READ) != FR_OK)
	{
		SendStringUART("Open file failed!\r\n");
		return false;
	}
	SendStringUART("File open!\r\n");
	//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	f_read(&wavFile, &chunkId, sizeof(chunkId), &readBytes);
	SendStringUART("chunkId: ");
	SendStringUART((char*)&chunkId);
	SendStringUART("\r\n");
	f_read(&wavFile, &chunkSize, sizeof(chunkSize), &readBytes);
	SendStringUART("chunkSize: ");
	SendStringUART((char*)&chunkSize);
	SendStringUART("\r\n");
	f_read(&wavFile, &format, sizeof(format), &readBytes);
	SendStringUART("format: ");
	SendStringUART((char*)&format);
	SendStringUART("\r\n");
	f_read(&wavFile, &subchunk1Id, sizeof(subchunk1Id), &readBytes);
	SendStringUART("subchunk1Id: ");
	SendStringUART((char*)&subchunk1Id);
	SendStringUART("\r\n");
	f_read(&wavFile, &subchunk1Size, sizeof(subchunk1Size), &readBytes);
	SendStringUART("subchunk1Size: ");
	SendStringUART((char*)&subchunk1Size);
	SendStringUART("\r\n");
	f_read(&wavFile, &audioFormat, sizeof(audioFormat), &readBytes);
	SendStringUART("audioFormat: ");
	SendStringUART((char*)&audioFormat);
	SendStringUART("\r\n");
	f_read(&wavFile, &numChannels, sizeof(numChannels), &readBytes);
	SendStringUART("numChannels: ");
	SendStringUART((char*)&numChannels);
	SendStringUART("\r\n");
	f_read(&wavFile, &sampleRate, sizeof(sampleRate), &readBytes);
	SendStringUART("sampleRate: ");
	SendStringUART((char*)&sampleRate);
	SendStringUART("\r\n");
	f_read(&wavFile, &byteRate, sizeof(byteRate), &readBytes);
	SendStringUART("byteRate: ");
	SendStringUART((char*)&byteRate);
	SendStringUART("\r\n");
	f_read(&wavFile, &blockAlign, sizeof(blockAlign), &readBytes);
	SendStringUART("blockAlign: ");
	SendStringUART((char*)&blockAlign);
	SendStringUART("\r\n");
	f_read(&wavFile, &bitsPerSample, sizeof(bitsPerSample), &readBytes);
	SendStringUART("bitsPerSample: ");
	SendStringUART((char*)&bitsPerSample);
	SendStringUART("\r\n");
	f_read(&wavFile, &subchunk2Id, sizeof(subchunk2Id), &readBytes);
	SendStringUART("subchunk2Id: ");
	SendStringUART((char*)&subchunk2Id);
	SendStringUART("\r\n");
	f_read(&wavFile, &subchunk2Size, sizeof(subchunk2Size), &readBytes);
	SendStringUART("subchunk2Size: ");
	SendStringUART((char*)&subchunk2Size);
	SendStringUART("\r\n");
	SendStringUART("File header loaded\r\n");
	//if(CHUNK_ID_CONST == wavFileInfo.chunkId && FORMAT_CONST == wavFileInfo.format && CHANNEL_STEREO == wavFileInfo.numChannels)
	if(CHUNK_ID_CONST == chunkId && FORMAT_CONST == format)
	{
		SendStringUART("Header is correct. WAV file\r\n");
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		//HAL_Delay(1000);
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
	}
	else
	{
		SendStringUART("Incorrect header\r\n");
		//HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
		return false;
	}
	return true;
}

void WAVPlayerPlay(I2S_HandleTypeDef* i2s)
{
	//utint
	/* Fill buffer first time */

	SendStringUART("Fill buffer first time\r\n");
	f_read(&wavFile, &audioBuffer, AUDIO_BUFFER_SIZE, &br);
	SendStringUART("Success\r\n");
	SendStringUART("Start DMA\r\n");
	HAL_I2S_Transmit_DMA(i2s, (uint16_t *)audioBuffer, AUDIO_BUFFER_SIZE);

/*
	while (1)
	{
		SendStringUART("Fill buffer\r\n");
		f_read(&wavFile, &audioBuffer, AUDIO_BUFFER_SIZE, &br);
		SendStringUART("Success\r\n");
		SendStringUART("Transmit via I2S\r\n");
		HAL_I2S_Transmit(i2s, (int16_t *)audioBuffer, AUDIO_BUFFER_SIZE, HAL_MAX_DELAY);
		SendStringUART("Success\r\n");
	}
*/
}

bool WAVPlayerStopAndCloseFile(void)
{
	if(f_close(&wavFile) == FR_OK)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WAVPlayerFillBuffer(I2S_HandleTypeDef* i2s, bool bufferHalf)
{
	if (bufferHalf == false)
	{
		HAL_I2S_Transmit_DMA(i2s, (uint16_t *)audioBuffer, AUDIO_BUFFER_SIZE);
		f_read(&wavFile, &audioBuffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2, &br);
	}
	else if (bufferHalf == true)
	{
		f_read(&wavFile, &audioBuffer[0], AUDIO_BUFFER_SIZE / 2, &br);
	}
}
