#include "FDevice.h"

#include "DirectXTK/SimpleMath.h"
#include <Debug/DebugConsole.h>
#include "Static/FEditorManager.h"
#include "Resource/Texture.h"
#include "Object/Window/Window.h"

void FDevice::Init(HWND _hwnd)
{
	CreateDeviceAndSwapChain(_hwnd);
	CreateFrameBuffer();
	CreateDepthStencilBuffer();
	
	InitResource();

	bIsInit = true;
}

void FDevice::Release()
{
	ReleaseDepthStencilBuffer();
	ReleaseFrameBuffer();
	ReleaseDeviceAndSwapChain();
	// 렌더 타겟을 초기화
	//FDevice::Get().GetDeviceContext()->OMSetRenderTargets(0, nullptr, DepthStencilView);
}

void FDevice::CreateFrameBuffer()
{
	// 스왑 체인으로부터 백 버퍼 텍스처 가져오기
	SwapChain->GetBuffer(0, IID_PPV_ARGS(&FrameBuffer));

	// 렌더 타겟 뷰 생성
	D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
	FrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 색상 포맷
	FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

	Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
}

void FDevice::CreateDeviceAndSwapChain(HWND hWindow)
{
	 // 지원하는 Direct3D 기능 레벨을 정의
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    
    // SwapChain 구조체 초기화
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = 0;                            // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Height = 0;                           // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 색상 포멧
    SwapChainDesc.SampleDesc.Count = 1;                            // 멀티 샘플링 비활성화
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 렌더 타겟으로 설정
    SwapChainDesc.BufferCount = 2;                                 // 더블 버퍼링
    SwapChainDesc.OutputWindow = hWindow;                          // 렌더링할 창 핸들
    SwapChainDesc.Windowed = TRUE;                                 // 창 모드
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // 스왑 방식

	//// 디바이스 생성 시 디버그 플래그 설정
	//UINT flags = D3D11_CREATE_DEVICE_DEBUG;
    
    // Direct3D Device와 SwapChain을 생성
    D3D11CreateDeviceAndSwapChain(
        // 입력 매개변수
        nullptr,                                                       // 디바이스를 만들 때 사용할 비디오 어댑터에 대한 포인터
        D3D_DRIVER_TYPE_HARDWARE,                                      // 만들 드라이버 유형을 나타내는 D3D_DRIVER_TYPE 열거형 값
        nullptr,                                                       // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,  // 사용할 런타임 계층을 지정하는 D3D11_CREATE_DEVICE_FLAG 열거형 값들의 조합
        FeatureLevels,                                                 // 만들려는 기능 수준의 순서를 결정하는 D3D_FEATURE_LEVEL 배열에 대한 포인터
        ARRAYSIZE(FeatureLevels),                                      // pFeatureLevels 배열의 요소 수
        D3D11_SDK_VERSION,                                             // SDK 버전. 주로 D3D11_SDK_VERSION을 사용
        &SwapChainDesc,                                                // SwapChain 설정과 관련된 DXGI_SWAP_CHAIN_DESC 구조체에 대한 포인터
    
        // 출력 매개변수
        &SwapChain,                                                    // 생성된 IDXGISwapChain 인터페이스에 대한 포인터
        &Device,                                                       // 생성된 ID3D11Device 인터페이스에 대한 포인터
        nullptr,                                                       // 선택된 기능 수준을 나타내는 D3D_FEATURE_LEVEL 값을 반환
        &DeviceContext                                                 // 생성된 ID3D11DeviceContext 인터페이스에 대한 포인터
    );
    
    // 생성된 SwapChain의 정보 가져오기
    SwapChain->GetDesc(&SwapChainDesc);
    
    // 뷰포트 정보 설정
	float InitWidth = (static_cast<float>(SwapChainDesc.BufferDesc.Width - SSplitter::DefaultSplitterWidth)) / 2;
	float InitHeight = (static_cast<float>(SwapChainDesc.BufferDesc.Height - SSplitter::DefaultSplitterWidth)) / 2;
	SetViewport(0, 0.0f, 0.0f, InitWidth, InitHeight);
	SetViewport(1, InitWidth, 0.0f, InitWidth + SSplitter::DefaultSplitterWidth, InitHeight);
	SetViewport(2, 0.0f, InitHeight, InitWidth, InitHeight + SSplitter::DefaultSplitterWidth);
	SetViewport(3, InitWidth, InitHeight, InitWidth, InitHeight + SSplitter::DefaultSplitterWidth);

}

void FDevice::ReleaseDeviceAndSwapChain()
{    if (DeviceContext)
{
	DeviceContext->Flush(); // 남이있는 GPU 명령 실행
}

	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

	ID3D11Debug* debugInterface = nullptr;
	Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugInterface);
	if (debugInterface) {
		debugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debugInterface->Release();
	}


	if (DeviceContext)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}


	if (Device)
	{
		Device->Release();
		Device = nullptr;
	}
}

void FDevice::SwapBuffer() const
{
	SwapChain->Present(1, 0); // SyncInterval: VSync 활성화 여부
}

void FDevice::OnUpdateWindowSize(int Width, int Height)
{
	// 프레임 버퍼를 해제
	ReleaseFrameBuffer();

	// 뎁스 스텐실 버퍼를 해제
	ReleaseDepthStencilBuffer();

	if (SwapChain)
	{
		SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);

		DXGI_SWAP_CHAIN_DESC SwapChainDesc;
		SwapChain->GetDesc(&SwapChainDesc);
		// 뷰포트 정보 갱신
		float InitWidth = (static_cast<float>(SwapChainDesc.BufferDesc.Width) - SSplitter::DefaultSplitterWidth) / 2;
		float InitHeight = (static_cast<float>(SwapChainDesc.BufferDesc.Height) - SSplitter::DefaultSplitterWidth) / 2;
		SetViewport(0, 0.0f, 0.0f, InitWidth + SSplitter::DefaultSplitterWidth, InitHeight);
		SetViewport(1, InitWidth, 0.0f, InitWidth + SSplitter::DefaultSplitterWidth, InitHeight);
		SetViewport(2, 0.0f, InitHeight, InitWidth, InitHeight + SSplitter::DefaultSplitterWidth);
		SetViewport(3, InitWidth, InitHeight, InitWidth, InitHeight + SSplitter::DefaultSplitterWidth);
	}
}

void FDevice::OnResizeComplete()
{
	// 프페임 버퍼를 재생성
	CreateFrameBuffer();

	// 깊이 스텐실 버퍼를 재생성
	CreateDepthStencilBuffer();
}



void FDevice::CreateDepthStencilBuffer()
{
	// 뷰포트 정보 설정
	D3D11_TEXTURE2D_DESC FrameBufferDesc;
	FrameBuffer->GetDesc(&FrameBufferDesc);

	D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
	DepthBufferDesc.Width = static_cast<float>(FrameBufferDesc.Width);
	DepthBufferDesc.Height = static_cast<float>(FrameBufferDesc.Height);
	DepthBufferDesc.MipLevels = 1;
	DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32비트 중 24비트는 깊이, 8비트는 스텐실
	DepthBufferDesc.SampleDesc.Count = 1;
	DepthBufferDesc.SampleDesc.Quality = 0;
	DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // 텍스쳐 바인딩 플래그를 DepthStencil로 설정
	DepthBufferDesc.CPUAccessFlags = 0;
	DepthBufferDesc.MiscFlags = 0;

	HRESULT result = Device->CreateTexture2D(&DepthBufferDesc, nullptr, &DepthStencilBuffer);

	Device->CreateTexture2D(&DepthBufferDesc, nullptr, &PickingDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DepthBufferDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
    
	if (DepthStencilBuffer == nullptr)
	{
		UE_LOG("DepthStencilBuffer is nullptr");
	}

	result = Device->CreateDepthStencilView(DepthStencilBuffer, &dsvDesc, &DepthStencilView);
	Device->CreateDepthStencilView(PickingDepthStencilBuffer, &dsvDesc, &PickingDepthStencilView);

	if (FAILED(result))
	{
		UE_LOG("Failed to create DepthStencilView");
	}

}

void FDevice::ReleaseFrameBuffer()
{
	if (FrameBuffer)
	{
		FrameBuffer->Release();
		FrameBuffer = nullptr;
	}

	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
}

void FDevice::ReleaseDepthStencilBuffer()
{
	if (DepthStencilBuffer)
	{
		DepthStencilBuffer->Release();
		DepthStencilBuffer = nullptr;
	}
	if (DepthStencilView)
	{
		DepthStencilView->Release();
		DepthStencilView = nullptr;
	}

	if (PickingDepthStencilBuffer)
	{
		PickingDepthStencilBuffer->Release();
		PickingDepthStencilBuffer = nullptr;
	}
	if (PickingDepthStencilView)
	{
		PickingDepthStencilView->Release();
		PickingDepthStencilView = nullptr;
	}
}

void FDevice::Prepare() const
{
	Clear();
	SetRenderTarget();
}

void FDevice::Clear() const
{
	// 스왑버퍼랑 뎁스스텐실 화면 지우기
	FDevice::Get().GetDeviceContext()->ClearRenderTargetView(FrameBufferRTV, ClearColor);
	FDevice::Get().GetDeviceContext()->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//UUID 텍스쳐 초기화
	FDevice::Get().GetDeviceContext()->ClearRenderTargetView(FEditorManager::Get().UUIDTexture->GetRTV(), PickingClearColor);

	//후처리 뎁스텍스쳐 초기화
	FDevice::Get().GetDeviceContext()->ClearDepthStencilView(PickingDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FDevice::SetRenderTarget() const
{
	///////////////////////
	///일단 임시로 여기서 UUID 픽킹 텍스쳐 바인딩


	ID3D11RenderTargetView* RTV = FEditorManager::Get().UUIDTexture->GetRTV();
	// 렌더 타겟 바인딩
	ID3D11RenderTargetView* RTVs[2] = { FrameBufferRTV, RTV };
	FDevice::Get().GetDeviceContext()->OMSetRenderTargets(2, RTVs, DepthStencilView);

	for (int i = 0; i < 4; i++)
	{
		// Rasterization할 Viewport를 설정 
		FDevice::Get().GetDeviceContext()->RSSetViewports(1, &ViewportInfo[i]);
	}



}

void FDevice::PickingPrepare() const
{

	ID3D11RenderTargetView* RTV = FEditorManager::Get().UUIDTexture->GetRTV();
	// 렌더 타겟 바인딩
	ID3D11RenderTargetView* RTVs[2] = { FrameBufferRTV, RTV };

	FDevice::Get().GetDeviceContext()->OMSetRenderTargets(2, RTVs, PickingDepthStencilView);
}

void FDevice::SetViewport(int index, float TopLeftX, float TopLeftY, float Width, float Height)
{
	if (index < 0 || index >= 4) return;

	ViewportInfo[index] = {
		TopLeftX, TopLeftY,
		Width, Height,
		0.0f, 1.0f
	};
}