#pragma once

#include "Core/Engine.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "StaticMesh/StaticMeshTypes.h"
#include "Debug/DebugConsole.h"
#include "DirectResource/ShaderResourceBinding.h"
#include "DirectResource/InputLayout.h"
#include "Mesh.h"
#include "Material.h"
#include "Core/Rendering/FDevice.h"
#include "Resource/ShaderConstants.h"

// 렌더 모드 열거형
enum class RenderMode
{
	Indexed,
	Instancing,
};

class FRenderResourceCollection
{
public:
	// 머티리얼 인덱스 상수 데이터
	FMatIndexConstantsComponentData MatIndexData;

	// 이름을 통해 Mesh와 Material 설정
	void SetMesh(const FString& _Name);
	void SetMaterial(const FString& _Name);

	// 포인터를 통해 Mesh와 Material 설정
	void SetMesh(std::shared_ptr<FMesh> _Mesh);
	void SetMaterial(std::shared_ptr<FMaterial> _Material);

	std::shared_ptr<FMesh> GetMesh() const { return Mesh; }
	std::shared_ptr<FMaterial> GetMaterial() const { return Material; }

	// 렌더링 함수와 리셋 함수
	void Render();
	void Reset();

	// Rasterizer override 여부 설정
	void SetIsOverrideRasterizer(bool _bOverrideRasterizer) { bOverrideRasterizer = _bOverrideRasterizer; }

	template<typename ConstantType>
	std::shared_ptr<class FConstantBufferBinding> SetConstantBufferBinding(const FString& _Name, const ConstantType* DataPtr,
		int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader)
	{
		return SetConstantBufferBinding(_Name, DataPtr, sizeof(ConstantType), _BindPoint, bIsUseVertexShader, bIsUsePixelShader);
	}

	// 상수 버퍼 바인딩
	std::shared_ptr<FConstantBufferBinding> SetConstantBufferBinding(const FString& _Name,
		const void* _CPUDataPtr, int _DataSize, int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader);

	// 텍스처 바인딩
	std::shared_ptr<class FTextureBinding> SetTextureBinding(const FString& _Name,
		int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader);

	// 샘플러 바인딩
	std::shared_ptr<class FSamplerBinding> SetSamplerBinding(const FString& _Name,
		int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader);

private:
	RenderMode Mode = RenderMode::Indexed;

	std::shared_ptr<class FInputLayout> Layout = nullptr;
	std::shared_ptr<FMesh> Mesh = nullptr;
	std::shared_ptr<FMaterial> Material = nullptr;

	// 바인딩 객체들을 저장
	TMap<FString, std::shared_ptr<FConstantBufferBinding>> ConstantBufferBindings;
	TMap<FString, std::shared_ptr<FTextureBinding>> TextureBindings;
	TMap<FString, std::shared_ptr<FSamplerBinding>> SamplerBindings;

	bool bOverrideRasterizer = true;
};
