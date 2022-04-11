#include "FbxLoader.h"
#include <cassert>

const std::string FbxLoader::baseDirectory = "Resources/";

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

void FbxLoader::LoadModelFromFile(const string& modelName)
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

    FbxModel* model = new FbxModel();
    model->name = modelName;

    ParseNodeRecursive(model, fbxScene->GetRootNode());

    fbxScene->Destroy();
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, FbxNode* fbxNode)
{
    //ノード名を取得
    string name = fbxNode->GetName();

    //子ノードに対して再度呼び出し
    for (int i = 0; i < fbxNode->GetChildCount(); i++)
    {
        ParseNodeRecursive(model, fbxNode->GetChild(i));
    }
}
