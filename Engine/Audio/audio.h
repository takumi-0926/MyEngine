#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")

using namespace std;
using namespace Microsoft::WRL;

struct SoundData
{
	//波形フォーマット
	WAVEFORMATEX wfex;
	//バッファの先頭アドレス
	BYTE* pbuffer;
	//バッファのサイズ
	unsigned int bufferSize;
};

//チャンクヘッダ
struct ChunkHeader
{
	char id[4];//チャンク毎のID
	int size;//チャンクサイズ
};

//RIFFヘッダチャンク
struct RiffHeader
{
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};

//FMTチャンク
struct FormatChunk
{
	ChunkHeader chunk;//"FMT"
	WAVEFORMATEX fmt;//波形フォーマット
};

class Audio {
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
private://メンバ変数
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	SoundData soundData[4];

	//音声データのロード
	SoundData SoundLoadWave(const char* filename);

	//音声データの解放(削除)
	void SoundUnLoad(SoundData* soundData);

	//音声データの再生
	void SoundPlayWave(IXAudio2* xaudio2, const SoundData& soundData);
public:
	bool Initalize();

	void Load();

	void Play(int number);

	~Audio();
};