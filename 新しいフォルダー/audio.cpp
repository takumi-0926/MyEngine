#include "audio.h"
#include <fstream>
#include <cassert>

//音声初期化
bool Audio::Initalize()
{
	auto result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	return true;
}

//音声データの再生
void Audio::Play(int number)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData[number].wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData[number].pbuffer;
	buf.AudioBytes = soundData[number].bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}

//音声データのロード
void Audio::Load()
{
	soundData[0] = SoundLoadWave("Resources/Alarm01.wav");
	soundData[1] = SoundLoadWave("Resources/gun_SE01.wav");
	soundData[2] = SoundLoadWave("Resources/bumb.wav");
	soundData[3] = SoundLoadWave("Resources/break.wav");
}

//音声データのロード
SoundData Audio::SoundLoadWave(const char* filename)
{
	HRESULT result;

	////ファイルオープン
	//ファイル入力ストリームのインスタンス
	ifstream file;

	//.wavファイルをバイナリモードで開く
	file.open(filename, ios_base::binary);

	//ファイルオープン失敗を検出する
	assert(file.is_open());

	////.wavデータの読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};

	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
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

	// Dataチャンクのデータ部（波形データ）の読み込み
	char* pbuffer = new char[data.size];
	file.read(pbuffer, data.size);

	//ファイルを閉じる
	file.close();

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pbuffer = reinterpret_cast<BYTE*>(pbuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

//音声データの解放(削除)
void Audio::SoundUnLoad(SoundData* soundData)
{
	delete[] soundData->pbuffer;

	soundData->pbuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

//音声データの再生
void Audio::SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData)
{
	HRESULT result;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf = {};
	buf.pAudioData = soundData.pbuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	assert(SUCCEEDED(result));

	result = pSourceVoice->Start();
	assert(SUCCEEDED(result));
}
