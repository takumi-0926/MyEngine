#include "audio.h"
#include <fstream>
#include <cassert>

//����������
bool Audio::Initalize()
{
	auto result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	return true;
}

//�����f�[�^�̍Đ�
void Audio::Play(int number)
{
	HRESULT result;

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[number].wfex);
	assert(SUCCEEDED(result));

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData[number].pbuffer;
	buf.AudioBytes = soundData[number].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

//�����f�[�^�̃��[�h
void Audio::Load()
{
	soundData[0] = SoundLoadWave("Resources/Alarm01.wav");
	soundData[1] = SoundLoadWave("Resources/gun_SE01.wav");
	soundData[2] = SoundLoadWave("Resources/bumb.wav");
	soundData[3] = SoundLoadWave("Resources/break.wav");
}

//�����f�[�^�̃��[�h
SoundData Audio::SoundLoadWave(const char* filename)
{
	HRESULT result;

	////�t�@�C���I�[�v��
	//�t�@�C�����̓X�g���[���̃C���X�^���X
	ifstream file;

	//.wav�t�@�C�����o�C�i�����[�h�ŊJ��
	file.open(filename, ios_base::binary);

	//�t�@�C���I�[�v�����s�����o����
	assert(file.is_open());

	////.wav�f�[�^�̓ǂݍ���
	//RIFF�w�b�_�[�̓ǂݍ���
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//�t�@�C����RIFF���`�F�b�N
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	//�^�C�v��WAVE���`�F�b�N
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Format�`�����N�̓ǂݍ���
	FormatChunk format = {};

	//�`�����N�w�b�_�[�̊m�F
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	//�`�����N�{�̂̓ǂݍ���
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Data�`�����N�̓ǂݍ���
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id, "JUNK", 4) == 0) {

		file.seekg(data.size, ios_base::cur);

		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}

	// Data�`�����N�̃f�[�^���i�g�`�f�[�^�j�̓ǂݍ���
	char* pbuffer = new char[data.size];
	file.read(pbuffer, data.size);

	//�t�@�C�������
	file.close();

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pbuffer = reinterpret_cast<BYTE*>(pbuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

//�����f�[�^�̉��(�폜)
void Audio::SoundUnLoad(SoundData* soundData)
{
	delete[] soundData->pbuffer;

	soundData->pbuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//�����f�[�^�̍Đ�
void Audio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
	HRESULT result;

	//�g�`�t�H�[�}�b�g������SourceVoice�̐���
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//�Đ�����g�`�f�[�^�̐ݒ�
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.pbuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// �g�`�f�[�^�̍Đ�
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}
