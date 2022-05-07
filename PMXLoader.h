//#pragma once
//#include <DirectXMath.h>
//#include <string>
//#include <vector>
//
//struct IKLink {
//	int index;
//
//	bool existAngleLimited;
//
//	bool limitAngleMin;
//
//	bool limitAngleMax;
//};
//
//�E�F�C�g
//struct Weight {
//	�E�F�C�g�^�C�v
//	enum Type {
//		BDEF1,//�{�[���̂�
//		BDEF2,//�{�[��2�ƃ{�[��1�̃E�F�C�g�l�iPMD�����j
//		BDEF4,//�{�[��4�Ƃ��ꂼ��̃E�F�C�g�l�i�E�F�C�g���v��1.0�̕ۏ؂Ȃ��j
//		SDEF,//BDEF2�ɉ����ASDEF�p��float3�iVector3�j��3�B�{���͕␳�l���K�v
//	};
//	Type type;
//
//	int born1;
//
//	int born2;
//
//	int born3;
//
//	int born4;
//
//	float weight1;
//
//	float weight2;
//
//	float weight3;
//
//	float weight4;
//
//	DirectX::XMFLOAT3 c;
//
//	DirectX::XMFLOAT3 r0;
//
//	DirectX::XMFLOAT3 r1;
//};
//
//�{�[��
//struct Bone {
//	std::wstring Name;
//
//	std::string EngName;
//
//	DirectX::XMFLOAT3 position;
//
//	int parentIndex;
//
//	int transformationLevel;
//
//	unsigned short flag;
//
//	DirectX::XMFLOAT3 coordOffset;
//
//	int childrenIndex;
//
//	int impartParentIndex;
//
//	float impartRate;
//
//	DirectX::XMFLOAT3 fixedAxis;
//
//	DirectX::XMFLOAT3 localAxisX;
//
//	DirectX::XMFLOAT3 localAxisY;
//
//	DirectX::XMFLOAT3 localAxisZ;
//
//	int externalparnetKey;
//
//	int ikTargetIndex;
//
//	int ikLoopCount;
//
//	float ikUnitAngle;
//
//	std::vector<IKLink> ikLinks;
//};
//
//���_���
//struct Vertex {
//	�ʒu
//	DirectX::XMFLOAT3 position;
//	�@��
//	DirectX::XMFLOAT3 normal;
//	UV
//	DirectX::XMFLOAT2 uv;
//	�ǉ�UV
//	std::vector<DirectX::XMFLOAT4> additional_uv;
//	�E�F�C�g
//	Weight weight;
//	�G�b�W�{��
//	float edge_diameter;
//	�C���f�b�N�X
//	unsigned short index;
//	�}�e���A��
//	Material materials;
//	�{�[��
//	Bone bone;
//};
//
//�}�e���A���i�ގ��j
//struct Material {
//	DirectX::XMFLOAT4 diffuse;
//
//	DirectX::XMFLOAT3 specular;
//
//	float specularStrength;
//
//	DirectX::XMFLOAT3 ambient;
//
//	int colorMaptextureIndex;
//
//	int toonTextureIndex;
//	�ގ����Ƃ̒��_��
//	int vertexNum;
//};
//���f���f�[�^
//struct PMXModelData {
//	Vertex vertex;
//
//	std::vector<Vertex> vertices;
//	std::vector<unsigned short> indices;
//};
//
//PMX�ǂݎ��
//bool LoadPmx(PMXModelData& data, const std::wstring& _filePath);
