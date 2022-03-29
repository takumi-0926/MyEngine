#pragma once
#include <DirectXMath.h>
#include "includes.h"

#include"LoadHlsl.h"

////3D�I�u�W�F�N�g�p���_�f�[�^
//struct  Vertex {
//	DirectX::XMFLOAT3 pos;
//	DirectX::XMFLOAT3 normal;
//	DirectX::XMFLOAT2 uv;
//};
//
////3D�I�u�W�F�N�g�\����
//struct Object3D {
//	ComPtr<ID3D12Resource> constBuff = nullptr;
//	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
//	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescHandleCBV;
//
//	//�A�t�B���ϊ��s��
//	XMFLOAT3 scale = { 1,1,1 };//�X�P�[�����O�{��
//	XMFLOAT3 rotation = { 0,0,0 };//��]�p
//	XMFLOAT3 position = { 0,0,0 };//���W
//
//	XMMATRIX worldMat;
//
//	Object3D* parent = nullptr;
//
//	bool Flag;
//};
//
////�V�F�[�_�ɓn�����߂̍s��f�[�^
//struct constBufferData {
//	XMFLOAT4 color;//�F(RGBA)
//	XMMATRIX mat;
//};
//
//const int constantBufferNum = 128;
//
//class Object3Ds : LoadHlsls
//{
//private:
//	ID3DBlob* _vsBlob = nullptr; //���_�V�F�[�_�p
//	ID3DBlob* _psBlob = nullptr; //�s�N�Z���V�F�[�_�p
//	ID3DBlob* _gsBlob = nullptr; //�W�I���g���V�F�[�_�p
//
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
//
//	ComPtr<ID3D12RootSignature>			_rootsignature;
//	ComPtr<ID3DBlob>					_rootSigBlob = nullptr;
//	ComPtr<ID3D12PipelineState>			_pipelinestate = nullptr;
//
//
//public:
//	//�p�C�v���C������
//	PipelineSet CreatePipeline(ID3D12Device* _dev);
//
//	//3D�I�u�W�F�N�g����
//	void CreateObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);
//
//	//3D�I�u�W�F�N�g�̏�����
//	void InitializeObject3D(Object3D* object, int index, ID3D12Device* _dev, ID3D12DescriptorHeap* descHeap);
//	//3D�I�u�W�F�N�g�̍X�V
//	void UpdateObject3D(Object3D* object, XMMATRIX& viewMat, XMMATRIX& projMat);
//	//3D�I�u�W�F�N�g�̕`��
//	void DrawObject3D(Object3D* object, ID3D12GraphicsCommandList* _cmdList,
//		ID3D12DescriptorHeap* descHeap, D3D12_VERTEX_BUFFER_VIEW& vbView,
//		D3D12_INDEX_BUFFER_VIEW& ibView, D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV, UINT numIndices);
//};