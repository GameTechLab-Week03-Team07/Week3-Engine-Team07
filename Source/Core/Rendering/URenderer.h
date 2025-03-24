#pragma once

#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>
#include "Core/Math/Vector.h"

class ACamera;

class URenderer
{
public:
	friend class FLineBatchManager;
public:
	
    /** Renderer를 초기화 합니다. */
    void Create(HWND hWindow);

    /** Renderer에 사용된 모든 리소스를 해제합니다. */
    void Release();

	void Render(class FRenderResourceCollection& InRenderResourceCollection);

	void LoadTexture(const wchar_t* texturePath);
	ID3D11ShaderResourceView* FontTextureSRV = nullptr;
	ID3D11SamplerState* FontSamplerState = nullptr;
    
protected:
    unsigned int Stride = 0;                                // Vertex 버퍼의 각 요소 크기

    ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // 기즈모용 스텐실 스테이트. Z버퍼 테스트 하지않고 항상 앞에렌더

	D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
public:
	FVector GetFrameBufferWindowSize() const;
};
