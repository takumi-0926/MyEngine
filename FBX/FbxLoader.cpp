#include "FbxLoader.h"
#include <cassert>

using namespace DirectX;

const std::string FbxLoader::baseDirectory = "Resources/";
const std::string FbxLoader::defaultTextureFileName = "white1x1.png";

FbxLoader* FbxLoader::GetInstance()
{
	static FbxLoader instance;
	return &instance;
}

void FbxLoader::Initialize(ID3D12Device* device)
{
	assert(fbxManager == nullptr);

	this->device = device;

	fbxManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);

	fbxImporter = FbxImporter::Create(fbxManager, "");
}

void FbxLoader::Finalize()
{
	fbxImporter->Destroy();
	fbxManager->Destroy();
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	//モデルと同じ名前のフォルダから読み込む
	const string directoryPath = baseDirectory +
		modelName + "/";

	//拡張子.fbxを付加
	const string fileName = modelName + ".fbx";

	//連結してフルパスを得る
	const string fullpath = directoryPath + fileName;

	//ファイル名を指定してFBXファイルを読み込む
	if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
	{
		assert(0);
	}

	FbxScene* fbxScene =
		FbxScene::Create(fbxManager, "fbxScene");

	fbxImporter->Import(fbxScene);

	//モデル生成
	FbxModel* model = new FbxModel();
	model->name = modelName;

	int nodeCount = fbxScene->GetNodeCount();

	model->nodes.reserve(nodeCount);

	ParseNodeRecursive(model, fbxScene->GetRootNode());

	model->fbxScene = fbxScene;

	//バッファ生成
	model->CreateBuffers(device);

	return model;
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode, Node* parent)
{
	model->nodes.emplace_back();
	Node& node = model->nodes.back();

	//ノード名を取得
	node.name = fbxNode->GetName();

	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	node.rotation = { (float)rotation[0],(float)rotation[1],(float)rotation[2],0.0f };
	node.scaling = { (float)scaling[0],(float)scaling[1],(float)scaling[2],0.0f };
	node.translation = { (float)translation[0],(float)translation[1],(float)translation[2],0.0f };

	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslation = XMMatrixTranslationFromVector(node.translation);

	node.transform = XMMatrixIdentity();
	node.transform *= matScaling;
	node.transform *= matRotation;
	node.transform *= matTranslation;

	//グローバル変形行列
	node.globleTransForm = node.transform;
	if (parent) {
		node.parent = parent;

		//親の変形を乗算
		node.globleTransForm = parent->globleTransForm;
	}

	//FBXノードからメッシュ情報を解析
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();
	if (fbxNodeAttribute) {
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh) {
			model->meshNode = &node;
			ParseMesh(model, fbxNode);
		}
	}

	//子ノードに対して再度呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(model, fbxNode->GetChild(i), &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, FbxNode* fbxNode)
{
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	ParseMeshVertices(model, fbxMesh);

	ParseMeshFaces(model, fbxMesh);

	ParseMaterial(model, fbxNode);

	ParseSkin(model, fbxMesh);
}

void FbxLoader::ParseMeshFaces(FbxModel* model, FbxMesh* fbxMesh)
{
	auto& vertices = model->vertices;
	auto& indices = model->indices;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);

	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();

	//UVデータの数
	const int textureUVCount = fbxMesh->GetTextureUVCount();

	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);

	//面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			//FBX頂点配列のインデックス
			int index = fbxMesh->GetPolygonVertex(i, j);
			assert(index >= 0);

			//頂点法線読み込み
			FbxModel::VertexPosNormalUvSkin& vertex = vertices[index];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal)) {
				vertex.normal.x = (float)normal[0];
				vertex.normal.y = (float)normal[1];
				vertex.normal.z = (float)normal[2];
			}

			//テクスチャUV読み込み
			if (textureUVCount > 0) {
				FbxVector2 uvs;
				bool lUnmappedUV;

				//0番決め打ちで読み込み
				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = (float)uvs[0];
					vertex.uv.y = (float)uvs[1];
				}
			}

			//インデックス配列に頂点インデックスを追加
			//3頂点まで
			if (j < 3) {
				indices.push_back(index);
			}
			//4頂点目
			else
			{
				//3点追加
				//四角形の0,1,2,3の内2,3,0で三角形を生成
				int index2 = indices[indices.size() - 1];
				int index3 = index;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}
		}
	}
}

void FbxLoader::ParseMeshVertices(FbxModel* model, FbxMesh* fbxMesh)
{
	auto& vertices = model->vertices;

	const int controlPointsCount =
		fbxMesh->GetControlPointsCount();

	FbxModel::VertexPosNormalUvSkin vert{};
	model->vertices.resize(controlPointsCount, vert);

	FbxVector4* pCoord =
		fbxMesh->GetControlPoints();

	for (int i = 0; i < controlPointsCount; i++)
	{
		FbxModel::VertexPosNormalUvSkin& vertex = vertices[i];

		vertex.pos.x = (float)pCoord[i][0];
		vertex.pos.y = (float)pCoord[i][1];
		vertex.pos.z = (float)pCoord[i][2];
	}
}

void FbxLoader::ParseMaterial(FbxModel* model, FbxNode* fbxNode)
{
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0) {
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);

		bool textureLoaded = false;

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				FbxSurfaceLambert* lambert =
					static_cast<FbxSurfaceLambert*>(material);

				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				model->ambient.x = (float)ambient.Get()[0];
				model->ambient.y = (float)ambient.Get()[1];
				model->ambient.z = (float)ambient.Get()[2];

				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				model->diffuse.x = (float)diffuse.Get()[0];
				model->diffuse.y = (float)diffuse.Get()[1];
				model->diffuse.z = (float)diffuse.Get()[2];

				const FbxProperty diffuseproperty =
					material->FindProperty(FbxSurfaceMaterial::sDiffuse);
				if (diffuseproperty.IsValid()) {
					const FbxFileTexture* texture =
						diffuseproperty.GetSrcObject<FbxFileTexture>();
					if (texture) {
						const char* filepath = texture->GetFileName();

						string path_str(filepath);
						string name = ExtractFileName(path_str);

						LoadTexture(model, baseDirectory + model->name + "/" + name);
						textureLoaded = true;
					}
				}
			}
		}

		if (!textureLoaded)
		{
			LoadTexture(model, baseDirectory + defaultTextureFileName);
		}
	}
}

void FbxLoader::LoadTexture(FbxModel* model, const std::string& fullPath)
{
	HRESULT result = S_FALSE;

	TexMetadata& metadata = model->metadata;
	ScratchImage& scrachImg = model->scrachImg;

	wchar_t wfilepath[128];
	MultiByteToWideChar(
		CP_ACP, 0, fullPath.c_str(), -1, wfilepath, _countof(wfilepath));
	result = LoadFromWICFile(
		wfilepath, WIC_FLAGS_NONE,
		&metadata, scrachImg
	);
	if (FAILED(result))
	{
		assert(0);
	}
}

void FbxLoader::ParseSkin(FbxModel* model, FbxMesh* fbxMesh)
{
	//スキニング情報
	FbxSkin* fbxSkin =
		static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));

	//スキニング情報がなければ終わり
	if (fbxSkin == nullptr)
	{
		return;
	}

	//ボーン配列の参照
	std::vector<FbxModel::Bone>& bones = model->bones;

	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//すべてのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体のノードの名前を取得
		const char* boneName = fbxCluster->GetLink()->GetName();

		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(FbxModel::Bone(boneName));
		FbxModel::Bone& bone = bones.back();

		//自作ボーンとfBXのボーンを紐ける
		bone.fbxCluster = fbxCluster;

		//FBXから初期姿勢行列を取得する
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix型に変換する
		XMMATRIX initialPose;
		ConvertMatrixFormFbx(&initialPose, fbxMat);

		//初期姿勢行列の逆行列を得る
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}

	//ボーン番号とスキンウェイトのペア
	struct WeightSet {
		UINT index;
		float weight;
	};

	//二次元配列(ジャグ配列)
	//list：頂点が影響を受けるボーンの全リスト
	//vector：それを全頂点分
	std::vector<std::list<WeightSet>>weightLists(model->vertices.size());

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//このボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int vertIndex = controlPointIndices[j];

			//スキンウェイト
			float weight = (float)controlPointWeights[j];

			//その頂点が影響を受けるボーンリストに、ボーンとウェイトのペアを追加
			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}

	//頂点配列書き換え用の参照
	auto& vertices = model->vertices;

	//各頂点についての処理
	for (int i = 0; i < vertices.size(); i++)
	{
		//頂点のウェイトからもっとも大きい４つを選択
		auto& weightList = weightLists[i];

		//大小比較用のラムダ式を指定して降順にソート
		weightList.sort(
			[](auto const& lhs, auto const& rhs) {
				return lhs.weight > rhs.weight;
			}
		);

		int weightArrayIndex = 0;
		//降順ソート済みのウェイトリストから
		for (auto& weightSet : weightList)
		{
			//頂点データに書き込み
			vertices[i].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[i].boneWeight[weightArrayIndex] = weightSet.weight;

			//４つに渡したら終了
			if (++weightArrayIndex >= FbxModel::MAX_BONE_INDICES)
			{
				float weight = 0.0f;

				//2番目以降のウェイトを合計
				for (int j = 0; j < FbxModel::MAX_BONE_INDICES; j++)
				{
					weight += vertices[i].boneWeight[j];
				}

				//合計で1.0f(100%)になるように調整
				vertices[i].boneWeight[0] = 1.0f - weight;
				break;
			}

		}
	}
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
	size_t pos1;

	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	return path;
}

void FbxLoader::ConvertMatrixFormFbx(DirectX::XMMATRIX* dst, const FbxMatrix& src)
{
	//行
	for (int i = 0; i < 4; i++)
	{
		//列
		for (int j = 0; j < 4; j++)
		{
			//1要素コピー
			dst->r[i].m128_f32[j] = (float)src.Get(i, j);
		}
	}
}
