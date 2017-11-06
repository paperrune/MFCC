#include <thread>
#include <Windows.h>

class Wav{
private:
	HWAVEIN hwavein;

	HWAVEOUT hwaveout;

	WAVEFORMATEX waveformatex;

	WAVEHDR wavehdr;
public:
	bool recording;

	char *buffer8;
	short *buffer16;
	float *buffer32;

	unsigned char *data;

	int length_buffer;
	int length_wav;

	Wav();
	Wav(char path[]);
	Wav(int nChannels, int nSamplesPerSec, int wBitsPerSample);
	~Wav();

	void BufferToWav();
	void WavToBuffer();
	void Create(unsigned char *data, int length_wav);
	void Load(char path[]);
	void Save(char path[]);
	void Get_Properties(int *nChannels, int *nSamplesPerSec, int *wBitsPerSample);
	void Set_Properties(int nChannels, int nSamplesPerSec, int wBitsPerSample);
	void Play();
	void Play(int milliseconds);
	void Record(int milliseconds);

	std::thread Record_Thread(int milliseconds){
		return std::thread([=]{ Record(milliseconds); });
	}
};