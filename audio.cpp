#include "audio.h"

HWAVEIN hWaveIn;
WAVEHDR WaveHeader;
short Buffer[BUFFER_SIZE];

static void CALLBACK waveInProc(HWAVEIN hwi, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (uMsg == WIM_DATA) {
		std::ofstream file(tempPath + audioFileName, std::ios::binary | std::ios::app);
		file.write((char*)Buffer, sizeof(Buffer));
		file.close();
		waveInAddBuffer(hWaveIn, &WaveHeader, sizeof(WAVEHDR));
	}
}


Result startRecord() {
	WAVEFORMATEX wf = {};
	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels = CHANNELS;
	wf.nSamplesPerSec = SAMPLE_RATE;
	wf.wBitsPerSample = BITS_PER_SAMPLE;
	wf.nBlockAlign = (CHANNELS * BITS_PER_SAMPLE) / 8;
	wf.nAvgBytesPerSec = SAMPLE_RATE * wf.nBlockAlign;

	if (waveInOpen(&hWaveIn, WAVE_MAPPER, &wf, (DWORD_PTR)waveInProc, 0, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
		return { COE::RecordAudioError, "Error opening microphone.", ResponseType::None, "" };
	}

	WaveHeader.lpData = (LPSTR)Buffer;
	WaveHeader.dwBufferLength = sizeof(Buffer);
	WaveHeader.dwFlags = 0;
	waveInPrepareHeader(hWaveIn, &WaveHeader, sizeof(WAVEHDR));
	waveInAddBuffer(hWaveIn, &WaveHeader, sizeof(WAVEHDR));

	waveInStart(hWaveIn);
	return { COE::Success, "", ResponseType::None, "" };
}