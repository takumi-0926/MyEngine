#pragma once
#include <Windows.h>
#include <xaudio2.h>
#include <wrl.h>

#pragma comment(lib,"xaudio2.lib")

using namespace std;
using namespace Microsoft::WRL;

struct SoundData
{
	//�g�`�t�H�[�}�b�g
	WAVEFORMATEX wfex;
	//�o�b�t�@�̐擪�A�h���X
	BYTE* pbuffer;
	//�o�b�t�@�̃T�C�Y
	unsigned int bufferSize;
};

//�`�����N�w�b�_
struct ChunkHeader
{
	char id[4];//�`�����N����ID
	int size;//�`�����N�T�C�Y
};

//RIFF�w�b�_�`�����N
struct RiffHeader
{
	ChunkHeader chunk;//"RIFF"
	char type[4];//"WAVE"
};

//FMT�`�����N
struct FormatChunk
{
	ChunkHeader chunk;//"FMT"
	WAVEFORMATEX fmt;//�g�`�t�H�[�}�b�g
};

class Audio {
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

private:
	// private�ȃR���X�g���N�^�i�V���O���g���p�^�[���j
	Audio() = default;
	// private�ȃf�X�g���N�^�i�V���O���g���p�^�[���j
	~Audio() = default;
	// �R�s�[�R���X�g���N�^���֎~�i�V���O���g���p�^�[���j
	Audio(const Audio& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	void operator=(const Audio& obj) = delete;

private://�����o�ϐ�
	ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice;
	SoundData soundData[4];

	//�����f�[�^�̃��[�h
	SoundData SoundLoadWave(const char* filename);

	//�����f�[�^�̉��(�폜)
	void SoundUnLoad(SoundData* soundData);

	//�����f�[�^�̍Đ�
	void SoundPlayWave(IXAudio2* xaudio2, const SoundData& soundData);

public:
	static Audio* GetInstance();

	bool Initalize();

	void Load();

	void Play(int number);
};