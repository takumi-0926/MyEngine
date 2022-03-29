#include "pmdObject3D.h"

bool PMDobject::StaticInitialize(ID3D12Device* device, SIZE ret)
{
    return false;
}

bool PMDobject::InitializeGraphicsPipeline()
{
    return false;
}

PMDobject* PMDobject::Create()
{
    return nullptr;
}

void PMDobject::SetModel(PMDobject* model)
{
}

bool PMDobject::Initialize()
{
    return false;
}

void PMDobject::Update()
{
}

void PMDobject::Draw()
{
}
