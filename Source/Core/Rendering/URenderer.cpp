#include "URenderer.h"
#include <d3dcompiler.h>
#include "DirectXTK/WICTextureLoader.h"
#include "FDevice.h"
#include "FViewMode.h"
#include "Debug/DebugConsole.h"
#include "Core/Math/Transform.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "Object/World/World.h"
#include "Object/Actor/Camera.h"
#include "Object/Assets/SceneAsset.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "Static/FUUIDBillBoard.h"
#include "Static/FLineBatchManager.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/PixelShader.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Resource/DirectResource/InputLayout.h"
#include "Resource/DirectResource/DepthStencilState.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/Rasterizer.h"
#include "Resource/DirectResource/ShaderResourceBinding.h"

void URenderer::Create(HWND hWindow)
{
	FViewMode::Get().Initialize(FDevice::Get().GetDevice());
	FLineBatchManager::Get().Create();
	FUUIDBillBoard::Get().Create();
}

void URenderer::Release()
{
	ID3D11RenderTargetView* ArrRenderTarget[8] = {nullptr,};
	
    // 렌더 타겟을 초기화
    FDevice::Get().GetDeviceContext()->OMSetRenderTargets(8, ArrRenderTarget, nullptr);
}

void URenderer::Render(FRenderResourceCollection& InRenderResourceCollection)
{
	InRenderResourceCollection.Render();
}

FVector URenderer::GetFrameBufferWindowSize() const
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	FDevice::Get().GetSwapChain()->GetDesc(&SwapChainDesc);

	return FVector(static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height), 0);
}