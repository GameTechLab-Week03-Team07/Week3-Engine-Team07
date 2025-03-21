#pragma once
#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>
#include "Core/AbstractClass/Singleton.h"

//디바이스 스왑 체인 관리, 뷰포트도 일단 가지고 있음

class FDevice : public TSingleton<FDevice>
{
	
public:
	void Init(HWND _hwnd);
	bool IsInit() const
	{
		return bIsInit;
	}

	inline ID3D11Device* GetDevice() const { return Device; }
	inline ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }
	inline const D3D11_VIEWPORT& GetViewPortInfo(int index) const { return ViewportInfo[index]; }
	inline ID3D11DepthStencilView* GetDepthStencilView() const { return DepthStencilView; }
	inline IDXGISwapChain* GetSwapChain() const { return SwapChain; }

	/** Renderer에 사용된 모든 리소스를 해제합니다. */
	void Release();
	
	/** 프레임 버퍼를 생성합니다. */
	void CreateFrameBuffer();
	void CreateDeviceAndSwapChain(HWND hWindow);
	/** Direct3D Device 및 SwapChain을 해제합니다.  */
	void ReleaseDeviceAndSwapChain();


	/** 뎁스 스텐실 버퍼를 생성합니다. */
	void CreateDepthStencilBuffer();

	/** 프레임 버퍼를 해제합니다. */
	void ReleaseFrameBuffer();

	void ReleaseDepthStencilBuffer();

	/** 렌더링 파이프라인을 준비 합니다. */
	void Prepare() const;

	void Clear() const;
	
	void SetRenderTarget() const;

	/** 픽킹렌더링 파이프라인을 준비 합니다. */
	void PickingPrepare() const;

	void SetViewport(int index, float TopLeftX, float TopLeftY, float Width, float Height);
	
	/** 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력 */
	void SwapBuffer() const;

	
	void OnUpdateWindowSize(int Width, int Height);

	void OnResizeComplete();

	//렌더러에 필요한 기본 리소스 생성
	void InitResource();

	
private:
	// Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
	ID3D11Device* Device = nullptr;                         // GPU와 통신하기 위한 Direct3D 장치
	ID3D11DeviceContext* DeviceContext = nullptr;           // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr;                    // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인


	FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면(스왑버퍼)을 초기화(clear)할 때 사용할 색상 (RGBA)

	FLOAT PickingClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //
	
    D3D11_VIEWPORT ViewportInfo[4] = {};                       // 렌더링 영역을 정의하는 뷰포트 정보

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr;                 // 화면 출력용 텍스처
	ID3D11RenderTargetView* FrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰

	// Depth Stenil Buffer
	ID3D11Texture2D* DepthStencilBuffer = nullptr;          // DepthStencil버퍼 역할을 하는 텍스쳐
	ID3D11DepthStencilView* DepthStencilView = nullptr;     // DepthStencil버퍼를 렌더 타겟으로 사용하는 뷰

	ID3D11Texture2D* PickingDepthStencilBuffer = nullptr;          // DepthStencil버퍼 역할을 하는 텍스쳐
	ID3D11DepthStencilView* PickingDepthStencilView = nullptr;     // DepthStencil버퍼를 렌더 타겟으로 사용하는 뷰

	bool bIsInit = FALSE;

	
};



