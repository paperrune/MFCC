#include <thread>
#include <Windows.h>

class Wav{
private:
	char *buffer8;
	short *buffer16;
	float *buffer32;

	HWAVEIN hwavein;

	HWAVEOUT hwaveout;

	WAVEFORMATEX waveformatex;

	WAVEHDR wavehdr;
public:
	bool recording;

	unsigned char *data;

	int length_buffer;
	int length_wav;

	Wav();
	Wav(char path[]);
	Wav(int nChannels, int nSamplesPerSec, int wBitsPerSample);
	~Wav();

	void BufferToWav();
	void WavToBuffer();
	void Create(unsigned char data[], int length_wav);
	void Create(int length_buffer, double buffer[]);
	void Load(char path[]);
	void Save(char path[]);
	void Get_Properties(int *nChannels, int *nSamplesPerSec, int *wBitsPerSample);
	void Set_Properties(int nChannels, int nSamplesPerSec, int wBitsPerSample);
	void Play();
	void Play(int milliseconds);
	void Record(int milliseconds);

	void Set_Buffer(int index, double value);
	double Get_Buffer(int index);

	std::thread Record_Thread(int milliseconds){
		return std::thread([=]{ Record(milliseconds); });
	}
};
