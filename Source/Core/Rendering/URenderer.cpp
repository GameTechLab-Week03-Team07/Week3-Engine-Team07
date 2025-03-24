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

// void URenderer::ReleasePickingFrameBuffer()
// {
// 	if (PickingFrameBuffer)
// 	{
// 		PickingFrameBuffer->Release();
// 		PickingFrameBuffer = nullptr;
// 	}
// 	if (PickingFrameBufferRTV)
// 	{
// 		PickingFrameBufferRTV->Release();
// 		PickingFrameBufferRTV = nullptr;
// 	}
// }

// void URenderer::CreatePickingTexture(HWND hWnd)
// {
//  //    RECT Rect;
//  //    int Width , Height;
//  //
//  //    Width =  FDevice::Get().GetViewPortInfo().Width;
// 	// Height =  FDevice::Get().GetViewPortInfo().Height;
//  //
//  //    D3D11_TEXTURE2D_DESC textureDesc = {};
//  //    textureDesc.Width = Width;
//  //    textureDesc.Height = Height;
//  //    textureDesc.MipLevels = 1;
//  //    textureDesc.ArraySize = 1;
//  //    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//  //    textureDesc.SampleDesc.Count = 1;
//  //    textureDesc.Usage = D3D11_USAGE_DEFAULT;
//  //    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//  //
//  //    FDevice::Get().GetDevice()->CreateTexture2D(&textureDesc, nullptr, &PickingFrameBuffer);
//  //
//  //    D3D11_RENDER_TARGET_VIEW_DESC PickingFrameBufferRTVDesc = {};
//  //    PickingFrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;      // 색상 포맷
//  //    PickingFrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처
//  //    
//  //    FDevice::Get().GetDevice()->CreateRenderTargetView(PickingFrameBuffer, &PickingFrameBufferRTVDesc, &PickingFrameBufferRTV);
// }

// void URenderer::PrepareZIgnore()
// {
//     FDevice::Get().GetDeviceContext()->OMSetDepthStencilState(IgnoreDepthStencilState, 0);
// }

// void URenderer::PreparePicking()
// {
//
// }

// void URenderer::PreparePickingShader() const
// {
//     FDevice::Get().GetDeviceContext()->PSSetShader(PickingPixelShader, nullptr, 0);
//
//     if (ConstantPickingBuffer)
//     {
//         FDevice::Get().GetDeviceContext()->PSSetConstantBuffers(1, 1, &ConstantPickingBuffer);
//     }
// }

// void URenderer::UpdateConstantPicking(FVector4 UUIDColor) const
// {
//     if (!ConstantPickingBuffer) return;
//
//     D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
//
//     UUIDColor = FVector4(UUIDColor.X/255.0f, UUIDColor.Y/255.0f, UUIDColor.Z/255.0f, UUIDColor.W/255.0f);
//     
//     FDevice::Get().GetDeviceContext()->Map(ConstantPickingBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
//     {
//         FPickingConstants* Constants = static_cast<FPickingConstants*>(ConstantBufferMSR.pData);
//         Constants->UUIDColor = UUIDColor;
//     }
//     FDevice::Get().GetDeviceContext()->Unmap(ConstantPickingBuffer, 0);
// }

// void URenderer::UpdateConstantDepth(int Depth) const
// {
//     // if (!ConstantsDepthBuffer) return;
//     //
//     // ACamera* Cam = FEditorManager::Get().GetCamera();
//     //
//     // D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
//     //
//     // FDevice::Get().GetDeviceContext()->Map(ConstantsDepthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
//     // {
//     //     FDepthConstants* Constants = static_cast<FDepthConstants*>(ConstantBufferMSR.pData);
//     //     Constants->DepthOffset = Depth;
//     //     Constants->nearPlane = Cam->GetNear();
//     //     Constants->farPlane = Cam->GetFar();
//     // }
//     // FDevice::Get().GetDeviceContext()->Unmap(ConstantsDepthBuffer, 0);
// }

// void URenderer::RenderPickingTexture()
// {
//     // 백버퍼로 복사
//     ID3D11Texture2D* backBuffer;
// 	FDevice::Get().GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
//     FDevice::Get().GetDeviceContext()->CopyResource(backBuffer, PickingFrameBuffer);
//     backBuffer->Release();
// }

FVector URenderer::GetFrameBufferWindowSize() const
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	FDevice::Get().GetSwapChain()->GetDesc(&SwapChainDesc);

	return FVector(static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height), 0);
}