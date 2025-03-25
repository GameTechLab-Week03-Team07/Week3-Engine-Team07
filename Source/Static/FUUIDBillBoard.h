#pragma once

#include "Object/Actor/Actor.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"
#include "Core/Rendering/FDevice.h"
#include "Debug/EngineShowFlags.h"
#include "DirectXTK/WICTextureLoader.h"

struct FFontConstantInfo
{
	FMatrix ViewProjectionMatrix;
};


class FUUIDBillBoard : public TSingleton<FUUIDBillBoard>
{
public:
	void Create();
	void Render();
	void SetTarget(AActor* Target);
	void UpdateString(const std::wstring& String);
	void CreateKoreanQuad(const wchar_t character, float& cursorX, int StringLength);
	void CreateKoreanConsonantVowel(wchar_t jamo, float posX, float offsetX, float offsetY);
private:
	void CalculateModelMatrix(FMatrix& OutMatrix);
	void Flush();
private:
	USceneComponent* TargetObject;

	TArray<FVertexSimple> VertexBuffer;
	TArray<uint32> IndexBuffer;

	// 최대 용량
	UINT MaxVerticesPerBatch = 256;
	UINT MaxIndicesPerBatch = 384;

	bool bShowDebugLines = true;
	bool bShowWorldGrid = true;

	// GPU 렌더링을 위한 버퍼
	ID3D11Buffer* FontVertexBuffer = nullptr;
	ID3D11Buffer* FontIndexBuffer = nullptr;
	ID3D11Buffer* FontConstantBuffer = nullptr;


	// 라인 렌더링 전용 쉐이더 리소스
	ID3D11VertexShader* FontVertexShader = nullptr;
	ID3D11PixelShader* FontPixelShader = nullptr;
	ID3D11InputLayout* FontInputLayout = nullptr;

	// 라인 렌더링 전용 상태
	D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 렌더링을 위한 특수 상태
	ID3D11RasterizerState* RasterizerState = nullptr;

	ID3D11DepthStencilState* DepthStencilState = nullptr;

	// Blend state
	ID3D11BlendState* BlendState = nullptr;
	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	ID3D11ShaderResourceView* FontTextureSRV = nullptr;
	ID3D11SamplerState* FontSamplerState = nullptr;
};