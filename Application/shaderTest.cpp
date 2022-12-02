#include "shaderTest.h"

ShaderTest::ShaderTest()
{
}

ShaderTest* ShaderTest::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	ShaderTest* object3d = new ShaderTest();
	if (object3d == nullptr) {
		return nullptr;
	}
	object3d->scale = { 5,5,5 };

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void ShaderTest::Update()
{
	Object3Ds::Update();
}

void ShaderTest::Draw()
{
	// nullptrチェック
	assert(device);
	assert(Object3Ds::cmdList);

	// オブジェクトモデルの割り当てがなければ描画しない
	if (model == nullptr) {
		return;
	}
	// パイプラインステートの設定
	cmdList->SetPipelineState(LoadHlsls::pipeline.at(ShaderNumber)._pipelinestate.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(LoadHlsls::pipeline.at(ShaderNumber)._rootsignature.Get());

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(0, constBuffB0->GetGPUVirtualAddress());

	// モデル描画
	model->Draw(cmdList.Get());
}
