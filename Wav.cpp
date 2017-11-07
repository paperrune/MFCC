#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "Wav.h"

#pragma comment(lib, "winmm.lib")

Wav::Wav(){
	ZeroMemory(&waveformatex, sizeof(waveformatex));
	length_buffer = 0;
	length_wav = 0;

	buffer8 = new char[length_buffer];
	buffer16 = new short[length_buffer];
	buffer32 = new float[length_buffer];
	data = new unsigned char[length_wav];
}
Wav::Wav(char path[]){
	ZeroMemory(&waveformatex, sizeof(waveformatex));
	length_buffer = 0;
	length_wav = 0;

	buffer8 = new char[length_buffer];
	buffer16 = new short[length_buffer];
	buffer32 = new float[length_buffer];
	data = new unsigned char[length_wav];

	Load(path);
}
Wav::Wav(int nChannels, int nSamplesPerSec, int wBitsPerSample){
	length_buffer = 0;
	length_wav = 0;

	buffer8 = new char[length_buffer];
	buffer16 = new short[length_buffer];
	buffer32 = new float[length_buffer];
	data = new unsigned char[length_wav];

	Set_Properties(nChannels, nSamplesPerSec, wBitsPerSample);
}
Wav::~Wav(){
	delete[] buffer8;
	delete[] buffer16;
	delete[] buffer32;
	delete[] data;
}

void Wav::BufferToWav(){
	switch (waveformatex.wBitsPerSample){
	case 8:
		data = (unsigned char*)realloc(data, sizeof(unsigned char)* (length_wav = length_buffer));

		for (int i = 0; i < length_buffer; i++){
			data[i] = (unsigned char)buffer8[i];
		}
		break;
	case 16:
		data = (unsigned char*)realloc(data, sizeof(unsigned char)* (length_wav = 2 * length_buffer));

		for (int i = 0; i < length_buffer; i++){
			data[2 * i + 1] = (unsigned char)(buffer16[i] >> 8);
			data[2 * i + 0] = (unsigned char)(buffer16[i] & 0xff);
		}
		break;
	case 32:
		data = (unsigned char*)realloc(data, sizeof(unsigned char)* (length_wav = 4 * length_buffer));

		for (int i = 0; i < length_buffer; i++){
			data[4 * i + 3] = (unsigned char)((*((unsigned long*)&buffer32[i]) >> 24));
			data[4 * i + 2] = (unsigned char)((*((unsigned long*)&buffer32[i]) >> 16) & 0xff);
			data[4 * i + 1] = (unsigned char)((*((unsigned long*)&buffer32[i]) >> 8) & 0xff);
			data[4 * i + 0] = (unsigned char)((*((unsigned long*)&buffer32[i]) & 0xff));
		}
		break;
	}
}
void Wav::WavToBuffer(){
	switch (waveformatex.wBitsPerSample){
	case 8:
		buffer8 = (char*)realloc(buffer8, sizeof(char)* (length_buffer = length_wav));

		for (int i = 0; i < length_buffer; i++){
			buffer8[i] = (char)data[i];
		}
		break;
	case 16:
		buffer16 = (short*)realloc(buffer16, sizeof(short)* (length_buffer = length_wav / 2));

		for (int i = 0; i < length_buffer; i++){
			buffer16[i] = (short)((data[2 * i + 1] << 8) | data[2 * i]);
		}
		break;
	case 32:
		buffer32 = (float*)realloc(buffer32, sizeof(float)* (length_buffer = length_wav / 4));

		for (int i = 0; i < length_buffer; i++){
			buffer32[i] = *(float*)&data[4 * i];
		}
		break;
	}
}
void Wav::Create(unsigned char *data, int length_wav){
	this->data = (unsigned char*)realloc(this->data, this->length_wav = length_wav);

	for (int i = 0; i < length_wav; i++){
		this->data[i] = data[i];
	}
}
void Wav::Load(char path[]){
	FILE *file = fopen(path, "rb");

	if (!file){
		fprintf(stderr, "[Load] [%s not found]\n", path);
		return;
	}

	int chunk;

	// RIFF
	fread(&chunk, sizeof(int), 1, file);
	if (chunk != 0x46464952){
		fprintf(stderr, "[Load] [not RIFF]\n");
	}
	fread(&chunk, 4, 1, file);
	fread(&chunk, 4, 1, file);
	if (chunk != 0x45564157){
		fprintf(stderr, "[Load] [not WAVE]\n");
	}

	// fmt 
	fread(&chunk, 4, 1, file);
	if (chunk != 0x20746d66){
		fprintf(stderr, "[Load] [not fmt]\n");
	}
	fread(&chunk, 4, 1, file);
	fread(&waveformatex.wFormatTag, 2, 1, file);
	fread(&waveformatex.nChannels, 2, 1, file);
	fread(&waveformatex.nSamplesPerSec, 4, 1, file);
	fread(&waveformatex.nAvgBytesPerSec, 4, 1, file);
	fread(&waveformatex.nBlockAlign, 2, 1, file);
	fread(&waveformatex.wBitsPerSample, 2, 1, file);

	fread(&chunk, 4, 1, file);
	if (chunk != 0x61746164){
		fprintf(stderr, "[Load] [not data]\n");
	}
	fread(&length_wav, sizeof(int), 1, file);
	data = (unsigned char*)realloc(data, length_wav);
	fread(data, length_wav, 1, file);
	fclose(file);
}
void Wav::Save(char path[]){
	FILE *file = fopen(path, "wb");

	int chunk;

	// RIFF
	fwrite(&(chunk = 0x46464952), 4, 1, file);
	fwrite(&(chunk = length_wav + 36), 4, 1, file);
	fwrite(&(chunk = 0x45564157), 4, 1, file);

	// fmt 
	fwrite(&(chunk = 0x20746d66), 4, 1, file);
	fwrite(&(chunk = 16), 4, 1, file);
	fwrite(&waveformatex.wFormatTag, 2, 1, file);
	fwrite(&waveformatex.nChannels, 2, 1, file);
	fwrite(&waveformatex.nSamplesPerSec, 4, 1, file);
	fwrite(&waveformatex.nAvgBytesPerSec, 4, 1, file);
	fwrite(&waveformatex.nBlockAlign, 2, 1, file);
	fwrite(&waveformatex.wBitsPerSample, 2, 1, file);

	// data
	fwrite(&(chunk = 0x61746164), 4, 1, file);
	fwrite(&(chunk = length_wav), 4, 1, file);
	fwrite(data, length_wav, 1, file);
	fclose(file);
}
void Wav::Get_Properties(int *nChannels, int *nSamplesPerSec, int *wBitsPerSample){
	if (nChannels) *nChannels = waveformatex.nChannels;
	if (nSamplesPerSec) *nSamplesPerSec = waveformatex.nSamplesPerSec;
	if (wBitsPerSample) *wBitsPerSample = waveformatex.wBitsPerSample;
}
void Wav::Set_Properties(int nChannels, int nSamplesPerSec, int wBitsPerSample){
	ZeroMemory(&waveformatex, sizeof(waveformatex));
	switch (wBitsPerSample){
	case 8:
		waveformatex.wFormatTag = WAVE_FORMAT_PCM;
		break;
	case 16:
		waveformatex.wFormatTag = WAVE_FORMAT_PCM;
		break;
	case 32:
		waveformatex.wFormatTag = 0x0003;
		break;
	default:
		fprintf(stderr, "[Set_Properties], [wBitsPerSample must be one of {8, 16, 32}.\n");
		return;
	}
	waveformatex.nChannels = nChannels;
	waveformatex.nSamplesPerSec = nSamplesPerSec;
	waveformatex.wBitsPerSample = wBitsPerSample;
	waveformatex.nBlockAlign = waveformatex.nChannels * waveformatex.wBitsPerSample / 8;
	waveformatex.nAvgBytesPerSec = waveformatex.nSamplesPerSec * waveformatex.nBlockAlign;
	waveformatex.cbSize = 0;
}
void Wav::Play(){
	Play(1000.0 * length_buffer / (waveformatex.nChannels * waveformatex.nSamplesPerSec));
}
void Wav::Play(int milliseconds){
	switch (waveformatex.wBitsPerSample){
	case 8:
		wavehdr.lpData = (LPSTR)buffer8;
		break;
	case 16:
		wavehdr.lpData = (LPSTR)buffer16;
		break;
	case 32:
		wavehdr.lpData = (LPSTR)buffer32;
		break;
	default:
		return;
	}
	wavehdr.dwBufferLength = waveformatex.wBitsPerSample / 8 * length_buffer;
	wavehdr.dwFlags = WHDR_DONE;
	wavehdr.dwLoops = 0;

	waveOutOpen(&hwaveout, WAVE_MAPPER, (tWAVEFORMATEX*)&waveformatex, 0, 0, 0);
	waveOutPrepareHeader(hwaveout, &wavehdr, sizeof(wavehdr));
	waveOutWrite(hwaveout, &wavehdr, sizeof(wavehdr));

	Sleep(milliseconds);

	waveOutPause(hwaveout);
	waveOutReset(hwaveout);
	waveOutUnprepareHeader(hwaveout, &wavehdr, sizeof(wavehdr));
	waveOutClose(hwaveout);
}
void Wav::Record(int milliseconds){
	recording = true;

	switch (waveformatex.wBitsPerSample){
	case 8:
		waveformatex.wFormatTag = WAVE_FORMAT_PCM;
		wavehdr.lpData = (LPSTR)(buffer8 = (char*)realloc(buffer8, sizeof(char)* (length_buffer = waveformatex.nSamplesPerSec * milliseconds / 1000.0)));
		break;
	case 16:
		waveformatex.wFormatTag = WAVE_FORMAT_PCM;
		wavehdr.lpData = (LPSTR)(buffer16 = (short*)realloc(buffer16, sizeof(short)* (length_buffer = waveformatex.nSamplesPerSec * milliseconds / 1000.0)));
		break;
	case 32:
		waveformatex.wFormatTag = 0x0003;
		wavehdr.lpData = (LPSTR)(buffer32 = (float*)realloc(buffer32, sizeof(float)* (length_buffer = waveformatex.nSamplesPerSec * milliseconds / 1000.0)));
		break;
	default:
		return;
	}
	wavehdr.dwBufferLength = waveformatex.wBitsPerSample / 8 * length_buffer;
	wavehdr.dwFlags = WHDR_DONE;
	wavehdr.dwLoops = 0;

	if (waveInOpen(&hwavein, WAVE_MAPPER, (tWAVEFORMATEX*)&waveformatex, 0, 0, 0) != MMSYSERR_NOERROR){
		fprintf(stderr, "[Record] [waveInOpen error]\n");
	}
	if (waveInPrepareHeader(hwavein, &wavehdr, sizeof(wavehdr)) != MMSYSERR_NOERROR){
		fprintf(stderr, "[Record] [waveInPrepareHeader error]\n");
		return;
	}
	waveInAddBuffer(hwavein, &wavehdr, sizeof(wavehdr));
	waveInStart(hwavein);

	for (int i = 0; i < milliseconds && recording; i++){
		Sleep(1);
	}

	waveInStop(hwavein);
	waveInReset(hwavein);
	if (waveInUnprepareHeader(hwavein, &wavehdr, sizeof(wavehdr)) != MMSYSERR_NOERROR){
		fprintf(stderr, "[Record] [waveInUnprepareHeader error]\n");
		return;
	}
	waveInClose(hwavein);
}

void Wav::Set_Buffer(int index, float value){
	switch (waveformatex.wBitsPerSample){
	case 8:
		buffer8[index] = value;
	case 16:
		buffer16[index] = value;
	case 32:
		buffer32[index] = value;
	}
}
double Wav::Get_Buffer(int index){
	if (length_buffer <= index || index < 0) return 0;

	switch (waveformatex.wBitsPerSample){
	case 8:
		return buffer8[index] / 128.0;
	case 16:
		return buffer16[index] / 32768.0;
	case 32:
		return buffer32[index];
	}
}
