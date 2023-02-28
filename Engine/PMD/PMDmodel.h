#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <string>
#include<vector>
#include <array>
#include <unordered_map>
#include "..\includes.h"
#include "..\Camera\Camera.h"

enum vmdData {
	WAIT = 0,
	WALK,
	DASH,
	ATTACK,
	DAMAGE,
	AVOID,
};

class PMDobject;
class Wrapper;
class PMDmodel {
	friend PMDobject;
	static Wrapper* dx12;
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//using TexMetadata = DirectX::TexMetadata;
	//using ScratchImage = DirectX::ScratchImage;

	//std::�ȗ�
	using string = std::string;

private:
	//�V�F�[�_�ɓ�������}�e���A���f�[�^
	struct MaterialForHlsl {
		XMFLOAT3 diffuse;//�f�B�t���[�Y�F
		float alpha;	//�f�B�t���[�Y��
		XMFLOAT3 specular;//�X�y�L�����F
		float specularStrength;//�X�y�L�����̋����i��Z�l�j
		XMFLOAT3 ambient;//�A���r�G���g�F
	};

	//��L�ȊO�̃}�e���A���f�[�^
	struct AdditionalMaterial {
		string texpath;
		int toonIdx = 0;
		bool edgeFlg = 0;
	};

	//�S�̂��܂Ƃ߂�f�[�^
	struct Material {
		unsigned int indicesNum = 0;
		MaterialForHlsl material = {};
		AdditionalMaterial additional = {};
	};
	Material material;

	struct Transform {
		void* operator new(size_t size);
		XMMATRIX world;
	};

	// �萔�o�b�t�@�p�f�[�^�\����B0
	struct ConstBufferDataB0_1
	{
		//XMFLOAT4 color;	// �F (RGBA)
		//XMMATRIX mat;	// �R�c�ϊ��s��
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX shadow;
		XMFLOAT3 cameraPos;
	};

	struct BoneNode {
		uint32_t boneIdx = 0;
		uint32_t boneType = 0;
		uint32_t ikParentBone = 0;
		XMFLOAT3 startPos = {};
		XMFLOAT3 endPos = {};
		std::vector<BoneNode*> children;
	};

	struct Motion {
		unsigned int frameNo;
		XMVECTOR quaternion;
		XMFLOAT3 offset;
		XMFLOAT2 p1, p2;
		Motion(
			unsigned int fno,
			XMVECTOR& q,
			XMFLOAT3& ofst,
			XMFLOAT2& ip1,
			const XMFLOAT2& ip2) :
			frameNo(fno),
			quaternion(q),
			offset(ofst),
			p1(ip1),
			p2(ip2) {
		}
	};

private:
	// �f�X�N���v�^�T�C�Y
	static UINT descriptorHandleIncrementSize;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap = nullptr;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;

	// ���_�o�b�t�@,�r���[
	ComPtr<ID3D12Resource> vertBuff = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// �C���f�b�N�X�o�b�t�@,�r���[
	ComPtr<ID3D12Resource> indexBuff = nullptr;
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	//���W�ϊ�
	Transform transform;
	Transform* mappedTransform = nullptr;
	ComPtr<ID3D12Resource> transformBuff = nullptr;
	ComPtr<ID3D12Resource> transformMat = nullptr;
	ComPtr<ID3D12DescriptorHeap> transformHeap = nullptr;

	//�}�e���A��
	uint32_t materialNum;
	std::vector<Material>materials;
	ComPtr<ID3D12Resource> materialBuff;
	std::vector<ComPtr<ID3D12Resource>> textureResources;
	std::vector<ComPtr<ID3D12Resource>> sphResources;
	std::vector<ComPtr<ID3D12Resource>> spaResources;
	std::vector<ComPtr<ID3D12Resource>> toonResources;

	//�q�[�v�̈�
	static ComPtr<ID3D12DescriptorHeap> materialDescHeap;

	//�{�[��
	struct vmdMotion {
		std::unordered_map<string, std::vector<Motion>> _motionData;
		unsigned int duration = 0;
		std::vector<DirectX::XMMATRIX> invBoneMatrices;
	};
#pragma pack(1)
	struct PMDBone {
		char boneName[20];
		unsigned short parentNo;
		unsigned short nextNo;
		unsigned char  type;
		unsigned short ikBoneNo;
		XMFLOAT3       pos;
	};
#pragma pack()

	map<unsigned int, vmdMotion> motion;

	vector<PMDBone> pmdBones;
	std::vector<DirectX::XMMATRIX> boneMatrices;
	std::map<std::string, BoneNode> _boneNodeTable;
	XMMATRIX* _mappedMatrices = nullptr;
	std::unordered_map<string, std::vector<Motion>> _motionData;

	//IK�{�[��
	struct PMDIK {
		uint16_t boneIdx = 0;;
		uint16_t targetidx = 0;
		uint16_t iterations = 0;;
		float limit = 0.0f;
		std::vector<uint16_t> nodeIdx;
	};
	std::vector<PMDIK> _ikData;

	std::vector<string> _boneNameArray;
	std::vector<BoneNode*> _boneNodeAddressArray;

	//std::vector<XMVECTOR> positions;
	//std::array<float,2> _edgeLens;
	std::vector<uint32_t> _kneeIdxes;

	struct VMDIKEnable {
		//�L�[�t���[��������t���[���ԍ�
		uint32_t freamNo = 0;;

		//���O�̃I��/�I�t�t���O�̃}�b�v
		std::unordered_map<std::string, bool> ikEnableTable;
	};
	std::vector<VMDIKEnable> _ikEnableData;

	DWORD _startTime;

	uint32_t indicesnum;

private:

	ComPtr<ID3D12Resource> LoadTextureFromFile(string& texPath);

	ID3D12Resource* CreateWhiteTexture();
	ID3D12Resource* CreateBlackTexture();

	HRESULT CreateMaterial();

	HRESULT CreateBone(FILE* fp);

	//�}�e���A���y�уe�N�X�`���̃r���[����
	HRESULT CreateMaterialAndTextureView();

	HRESULT CreateTransform();

	HRESULT CreateDescHeap();

	HRESULT LoadPMDFile(const char* path);

	void recursiveMatrixMultiply(BoneNode* node, const XMMATRIX& mat);

	void MotionUpdate();

	float GetYFromXOn(float x, const XMFLOAT2& a, const XMFLOAT2& b, uint8_t n);

	//CCDIK�ɂ��{�[������������
	void SolveCCOIK(const PMDIK& ik);
	//�]���藝IK
	void SolveConsineIK(const PMDIK& ik);
	//LookAt�s��
	void SolveLookAt(const PMDIK& ik);
	//�ꍇ����
	void IKSolve(uint32_t frameNo);

public:
	PMDmodel(/*Wrapper* _dx12, const char* filepath, PMDobject& object*/);
	~PMDmodel();

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="_dx12">DirectX��{���</param>
	/// <returns>�����Ftrue / ���s�Ffalse</returns>
	static bool StaticInitialize(Wrapper* _dx12);
	/// <summary>
	/// �C���X�^���X�����y��PMD�t�@�C���ǂݍ���
	/// </summary>
	/// <param name="filepath">�t�@�C���p�X</param>
	/// <returns>�C���X�^���X</returns>
	static PMDmodel* CreateFromPMD(const char* filepath);

	bool Initialize(const char* filepath);

	void Update();
	void UpdateWorldMatrix();

	void Draw(ID3D12GraphicsCommandList* cmdList, bool isShadow = false);

	void playAnimation();

	HRESULT LoadVMDFile(const unsigned int Number, const char* path);

	//�A�N�Z�b�T
	ID3D12DescriptorHeap* MaterialDescHeap() { return materialDescHeap.Get(); }
	ID3D12DescriptorHeap* DescHeap() { return descHeap.Get(); }
	D3D12_VERTEX_BUFFER_VIEW VbView() { return vbView; }
	D3D12_INDEX_BUFFER_VIEW IbView() { return ibView; }
	std::vector<Material> Materials() { return materials; }
	XMFLOAT3 GetBonePos(std::string KeyName) {
		PMDBone bone;
		BoneNode _bone;
		_bone = _boneNodeTable.at(KeyName);
		bone = pmdBones.at(94);
		return _bone.startPos;
		//return bone.pos;
	}
	std::vector<DirectX::XMMATRIX> GetBoneMat() { return boneMatrices; }
	XMMATRIX GetWorldMat() {
		// �X�P�[���A��]�A���s�ړ��s��̌v�Z
		XMMATRIX matScale, matRot, matTrans;
		matScale = XMMatrixScaling(this->scale.x, this->scale.y, this->scale.z);
		matRot = XMMatrixIdentity();
		matRot *= XMMatrixRotationZ(XMConvertToRadians(this->rotation.z));
		matRot *= XMMatrixRotationX(XMConvertToRadians(this->rotation.x));
		matRot *= XMMatrixRotationY(XMConvertToRadians(this->rotation.y));
		matTrans = XMMatrixTranslation(this->position.x, this->position.y, this->position.z);

		// ���[���h�s��̍���
		matWorld = XMMatrixIdentity(); // �ό`�����Z�b�g
		matWorld *= matScale; // ���[���h�s��ɃX�P�[�����O�𔽉f
		matWorld *= matRot; // ���[���h�s��ɉ�]�𔽉f
		matWorld *= matTrans; // ���[���h�s��ɕ��s�ړ��𔽉f

		return matWorld;
	}

	void SetPosition(XMFLOAT3 position) {
		this->position = position;
	}
	XMFLOAT3 GetPosition() {
		return position;
	}
	void SetMatRot(XMMATRIX rot) { this->matRot = rot; }

public://�����o�ϐ�
	ComPtr<ID3D12Resource> PMDconstBuffB1; // �萔�o�b�t�@

	// �F
	XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	XMMATRIX matWorld;

	XMMATRIX matRot = XMMatrixIdentity();;

	bool animation = false;
	//�I�𒆂̃��[�V�����f�[�^
	int vmdNumber = 0;
	int oldVmdNumber = 0;
	bool a = false;
};