#include "DebugImgui.h"

float DebugImgui::shadowCameraSite[2] = { 320.0f,320.0f };
float DebugImgui::shadowlightPos[3] = { 30.0f,50.0f,20.0f };
float DebugImgui::shadowlightTarget[3] = { 0.0f,0.0f,40.0f };
float DebugImgui::shadowlightLange[2] = { -5.0f,100.0f };

DebugImgui::DebugImgui()
{
}

DebugImgui::~DebugImgui()
{
}

void DebugImgui::UpdateImgui(Wrapper* dx12)
{
	//深度テクスチャID取得
	auto HeapGPUHandle = dx12->GetDescHeap()->GetGPUDescriptorHandleForHeapStart();
	//HeapGPUHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	ImTextureID texID = ImTextureID(HeapGPUHandle.ptr += dx12->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * 2);

	ImGui::Begin("ShadowCameraTest");
	ImGui::SetWindowSize(ImVec2(400, 500), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::InputFloat2("cameraSite", shadowCameraSite);
	ImGui::InputFloat3("cameraPos", shadowlightPos);
	ImGui::InputFloat3("cameraTarget", shadowlightTarget);
	ImGui::InputFloat2("cameraLange", shadowlightLange);
	ImGui::Image(texID, ImVec2(shadowCameraSite[0], shadowCameraSite[1]));
	ImGui::End();
}