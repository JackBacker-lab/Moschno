#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <iostream>
#include "globals.h"

#pragma comment(lib, "winmm.lib")

#define SAMPLE_RATE 44100  // Sampling frequency (Hz)
#define BITS_PER_SAMPLE 16 // Audio bit depth
#define CHANNELS 2         // 1 - mono, 2 - stereo
#define BUFFER_SIZE 44100  // 1 second of audio

extern HWAVEIN hWaveIn;
extern WAVEHDR WaveHeader;
extern short Buffer[BUFFER_SIZE];

Result startRecord();

#endif
