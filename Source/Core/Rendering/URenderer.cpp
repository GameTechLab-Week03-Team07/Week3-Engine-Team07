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
#include "Static/ViewportClient.h"
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
    //CreateDeviceAndSwapChain(hWindow);
    //CreateFrameBuffer();
    //CreatePickingTexture(hWindow);

	FViewMode::Get().Initialize(FDevice::Get().GetDevice());
	FLineBatchManager::Get().Create();
	FUUIDBillBoard::Get().Create();
	FViewportClient::Get().Create();

	//LoadTexture(L"font_atlas.png");
	LoadTexture(L"Pretendard_Kor.png");
}

void URenderer::Release()
{
   // ReleaseDepthStencilBuffer();

	ID3D11RenderTargetView* ArrRenderTarget[8] = {nullptr,};
	
    // 렌더 타겟을 초기화
    FDevice::Get().GetDeviceContext()->OMSetRenderTargets(8, ArrRenderTarget, nullptr);
}

// void URenderer::CreateShader()
// {
//     ID3DBlob* PickingShaderCSO;
// 	ID3DBlob* ErrorMsg = nullptr;
//
//     D3DCompileFromFile(L"Shaders/ShaderW0.hlsl", nullptr, nullptr, "PickingPS", "ps_5_0", 0, 0, &PickingShaderCSO, nullptr);
//     FDevice::Get().GetDevice()->CreatePixelShader(PickingShaderCSO->GetBufferPointer(), PickingShaderCSO->GetBufferSize(), nullptr, &PickingPixelShader);
//   
// 	if (ErrorMsg)
// 	{
// 		std::cout << (char*)ErrorMsg->GetBufferPointer() << std::endl;
// 		ErrorMsg->Release();
// 	}
//
//     PickingShaderCSO->Release();
// }
//
// void URenderer::CreateConstantBuffer()
// {
//
//
//     // D3D11_BUFFER_DESC ConstantBufferDescPicking = {};
//     // ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
//     // ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
//     // ConstantBufferDescPicking.ByteWidth = sizeof(FPickingConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
//     // ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
//     //
//     // FDevice::Get().GetDevice()->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantPickingBuffer);
//     //
//     // D3D11_BUFFER_DESC ConstantBufferDescDepth = {};
//     // ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
//     // ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
//     // ConstantBufferDescPicking.ByteWidth = sizeof(FDepthConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
//     // ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
//     //
//     // FDevice::Get().GetDevice()->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantsDepthBuffer);
// }

void URenderer::ReleaseConstantBuffer()
{

    //
    // if (ConstantPickingBuffer)
    // {
    //     ConstantPickingBuffer->Release();
    //     ConstantPickingBuffer = nullptr;
    // }

    // if (ConstantsDepthBuffer)
    // {
    //     ConstantsDepthBuffer->Release();
    //     ConstantsDepthBuffer = nullptr;
    // }
}

// void URenderer::Prepare() const
// {
// }

// void URenderer::PrepareShader() const
// {
//
//     if (ConstantsDepthBuffer)
//     {
//         FDevice::Get().GetDeviceContext()->PSSetConstantBuffers(2, 1, &ConstantsDepthBuffer);
//     }
// }

void URenderer::Render(FRenderResourceCollection& InRenderResourceCollection)
{
	InRenderResourceCollection.Render();
}


void URenderer::LoadTexture(const wchar_t* texturePath)
{
	DirectX::CreateWICTextureFromFile(FDevice::Get().GetDevice(), FDevice::Get().GetDeviceContext(), texturePath, nullptr, &FontTextureSRV);

	
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	FDevice::Get().GetDevice()->CreateSamplerState(&samplerDesc, &FontSamplerState);
	FDevice::Get().GetDeviceContext()->PSSetShaderResources(0, 1, &FontTextureSRV);
	FDevice::Get().GetDeviceContext()->PSSetSamplers(0, 1, &FontSamplerState);
}




// void URenderer::CreateDepthStencilState()
// {
// 	
//     D3D11_DEPTH_STENCIL_DESC IgnoreDepthStencilDesc = {};
//     IgnoreDepthStencilDesc.DepthEnable = TRUE;
//     IgnoreDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//     IgnoreDepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;                     
//     FDevice::Get().GetDevice()->CreateDepthStencilState(&IgnoreDepthStencilDesc ,&IgnoreDepthStencilState);
// }


// void URenderer::ReleaseDepthStencilBuffer()
// {
//     if (IgnoreDepthStencilState)
//     {
//         IgnoreDepthStencilState->Release();
//         IgnoreDepthStencilState = nullptr;
//     }
// }



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

// void URenderer::PrepareMain()
// {
// 	//FDevice::Get().GetDeviceContext()->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
//     //FDevice::Get().GetDeviceContext()->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
//     FDevice::Get().GetDeviceContext()->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
// }



//void URenderer::OnUpdateWindowSize(uint32 Width, uint32 Height)
//{
//	
//	//ReleasePickingFrameBuffer();
//}

//void URenderer::OnResizeComplete()
//{
//	//CreatePickingTexture(UEngine::Get().GetWindowHandle());
//	// 깊이 스텐실 버퍼를 재생성
//}

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