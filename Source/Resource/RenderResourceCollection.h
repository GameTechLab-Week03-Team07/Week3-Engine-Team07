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
#include "Resource/FMatIndexConstantsComponentData.h"
enum class RenderMode
{
	Indexed,
	Instancing,
};


class FRenderResourceCollection
{
public:

	FMatIndexConstantsComponentData MatIndexData;
	//테스트 임시 메쉬



	void SetMesh(const FString& _Name);
	void SetMaterial(const FString& _Name);

	
	void SetMesh(std::shared_ptr<class FMesh> _Mesh);
	void SetMaterial(std::shared_ptr<class FMaterial> _Material);

	std::shared_ptr<FMesh> GetMesh() const
	{
		return Mesh;
	}

	std::shared_ptr<FMaterial> GetMaterial() const
	{
		return Material;
	}

	void Render();
	void Reset();



	template<typename ConstantType>
	std::shared_ptr<class FConstantBufferBinding> SetConstantBufferBinding(const FString& _Name,  const ConstantType* DataPtr,
	 int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader)
	{
		return SetConstantBufferBinding(_Name, DataPtr,sizeof(ConstantType),_BindPoint,  bIsUseVertexShader, bIsUsePixelShader);
	}
	
	
	std::shared_ptr<FConstantBufferBinding> SetConstantBufferBinding(const FString& _Name,
	                                                                 const void* _CPUDataPtr, int _DataSize,int _BindPoint, bool	bIsUseVertexShader, bool bIsUsePixelShader);
	
	
	std::shared_ptr<class FTextureBinding> SetTextureBinding(const FString& _Name,
		int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader);

	
	std::shared_ptr<class FSamplerBinding> SetSamplerBinding(const FString& _Name,
		int _BindPoint,	bool bIsUseVertexShader, bool bIsUsePixelShader);
	
private:
	//class UPrimitiveComponent* ParentRenderer = nullptr;

	RenderMode Mode = RenderMode::Indexed;

	//원래는 메쉬와 메테리얼의 정보 둘다 필요함 일단 버텍스 쉐이더 정보만 받아서 자동화
	std::shared_ptr<class FInputLayout> Layout = nullptr;
	
	std::shared_ptr<FMesh> Mesh = nullptr;
	std::shared_ptr<FMaterial> Material = nullptr;

	TMap<FString, std::shared_ptr<FConstantBufferBinding>> ConstantBufferBindings;
	TMap<FString, std::shared_ptr<FTextureBinding>> TextureBindings;
	TMap<FString, std::shared_ptr<FSamplerBinding>> SamplerBindings;
	
	void UpdateMatIndexConstantBuffer(int MatIndex);
	// // 테스트 상수버퍼
	// std::shared_ptr<class FConstantBufferBinding> ConstantBufferBinding = nullptr;
	// std::shared_ptr<class FConstantBuffer> ConstantBuffer = nullptr;
	
};