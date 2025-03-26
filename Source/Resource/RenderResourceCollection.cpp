#include "RenderResourceCollection.h"

// 리소스 바인딩 생성을 위한 헬퍼 템플릿
namespace {
	template<typename BindingType, typename ResourceType>
	std::shared_ptr<BindingType> CreateResourceBinding(
		const FString& Name,
		int BindPoint,
		bool bUseVertexShader,
		bool bUsePixelShader,
		TMap<FString, std::shared_ptr<BindingType>>& BindingMap,
		std::shared_ptr<ResourceType>(*FindFunc)(const FString&)
	)
	{
		std::shared_ptr<ResourceType> Res = FindFunc(Name);
		if (!Res)
		{
			return nullptr;
		}
		auto Binding = std::make_shared<BindingType>();
		Binding->Res = Res;
		Binding->Name = Name;
		Binding->bIsUseVertexShader = bUseVertexShader;
		Binding->bIsUsePixelShader = bUsePixelShader;
		Binding->BindPoint = BindPoint;
		BindingMap.Add(Name, Binding);
		return Binding;
	}
}

void FRenderResourceCollection::SetMesh(const FString& _Name)
{
	Mesh = FMesh::Find(_Name);
	SetMesh(Mesh);
}

void FRenderResourceCollection::SetMaterial(const FString& _Name)
{
	Material = FMaterial::Find(_Name);
	SetMaterial(Material);
}

void FRenderResourceCollection::SetMesh(std::shared_ptr<FMesh> _Mesh)
{
	Mesh = _Mesh;
	if (Mesh == nullptr)
	{
		MsgBoxAssert("존재하지 않는 매쉬를 세팅하려고 했습니다.");
	}
	if (Layout == nullptr && Material != nullptr)
	{
		Layout = std::make_shared<FInputLayout>();
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::SetMaterial(std::shared_ptr<FMaterial> _Material)
{
	Material = _Material;
	if (Material == nullptr)
	{
		MsgBoxAssert("존재하지 않는 머티리얼을 세팅하려고 했습니다.");
	}
	if (Layout == nullptr && Mesh != nullptr)
	{
		Layout = std::make_shared<FInputLayout>();
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::Render()
{
	Mesh->Setting();
	Layout->Setting();

	for (auto& Binding : ConstantBufferBindings)
	{
		Binding.Value->Setting();
	}

	for (auto& Binding : TextureBindings)
	{
		Binding.Value->Setting();
	}

	for (auto& Binding : SamplerBindings)
	{
		Binding.Value->Setting();
	}

	// 메시의 섹션 정보가 있는 경우 섹션별 렌더링 처리
	if (Mesh->GetSections().Num() != 0)
	{
		for (const auto& Section : Mesh->GetSections())
		{
			std::shared_ptr<FMaterial> sectionMat = FMaterial::Find(Section.SlotName);
			if (!sectionMat)
			{
				sectionMat = FMaterial::Find("StaticMeshMaterial");
			}
			MatIndexData.MatIndex = Section.MaterialIndex;
			for (auto& Binding : ConstantBufferBindings)
			{
				Binding.Value->Setting();
			}
			sectionMat->Setting(bOverrideRasterizer);
			FDevice::Get().GetDeviceContext()->DrawIndexed(Section.indexCount, Section.indexStart, 0);
		}
	}
	else
	{
		Material->Setting(bOverrideRasterizer);
		Mesh->Draw();
	}
}

void FRenderResourceCollection::Reset()
{
	for (auto& Binding : TextureBindings)
	{
		Binding.Value->Reset();
	}
}

std::shared_ptr<FTextureBinding> FRenderResourceCollection::SetTextureBinding(
	const FString& _Name,
	int _BindPoint,
	bool bIsUseVertexShader,
	bool bIsUsePixelShader)
{
	return CreateResourceBinding<FTextureBinding, FTexture>(
		_Name, _BindPoint, bIsUseVertexShader, bIsUsePixelShader,
		TextureBindings, FTexture::Find);
}

std::shared_ptr<FSamplerBinding> FRenderResourceCollection::SetSamplerBinding(
	const FString& _Name,
	int _BindPoint,
	bool bIsUseVertexShader,
	bool bIsUsePixelShader)
{
	return CreateResourceBinding<FSamplerBinding, FSampler>(
		_Name, _BindPoint, bIsUseVertexShader, bIsUsePixelShader,
		SamplerBindings, FSampler::Find);
}

std::shared_ptr<FConstantBufferBinding> FRenderResourceCollection::SetConstantBufferBinding(
	const FString& _Name,
	const void* _CPUDataPtr,
	int _DataSize,
	int _BindPoint,
	bool bIsUseVertexShader,
	bool bIsUsePixelShader)
{
	std::shared_ptr<FConstantBuffer> Res = FConstantBuffer::Find(_Name);
	if (Res == nullptr)
	{
		Res = FConstantBuffer::Create(_Name, _DataSize);
	}

	auto Binding = std::make_shared<FConstantBufferBinding>();
	Binding->DataSize = _DataSize;
	Binding->CPUDataPtr = _CPUDataPtr;
	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	ConstantBufferBindings.Add(_Name, Binding);

	return Binding;
}
