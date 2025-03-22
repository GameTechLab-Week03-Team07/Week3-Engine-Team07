#include "RenderResourceCollection.h"
#include "Debug/DebugConsole.h"
#include "DirectResource/ShaderResourceBinding.h"
#include "DirectResource/InputLayout.h"
#include "Mesh.h"
#include "Material.h"

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

	if (nullptr == Mesh)
	{
		MsgBoxAssert("존재하지 않는 매쉬를 세팅하려고 했습니다.");
	}

	if (nullptr == Layout && nullptr != Material)
	{
		Layout = std::make_shared<FInputLayout>();
		
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::SetMaterial(std::shared_ptr<FMaterial> _Material)
{
	Material = _Material;


	if (nullptr == Material)
	{
		MsgBoxAssert("존재하지 않는 머티리얼을 세팅하려고 했습니다.");
	}

	if (nullptr == Layout && nullptr != Mesh)
	{
		Layout = std::make_shared<FInputLayout>();
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::Render()
{
	Mesh->Setting();
	Layout->Setting();
	Material->Setting(); 

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
	
	Mesh->Draw();
}

void FRenderResourceCollection::Reset()
{
	for (auto& Binding : TextureBindings)
	{
		Binding.Value->Reset();
	}
}

std::shared_ptr<FTextureBinding> FRenderResourceCollection::SetTextureBinding(const FString& _Name, int _BindPoint,
                                                                              bool bIsUseVertexShader, bool bIsUsePixelShader)
{
	std::shared_ptr<FTexture> Res = FTexture::Find(_Name);

	// if (Res == nullptr)
	// {
	// 	//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
	// 	Res = FConstantBuffer::Create(_Name, _DataSize);
	// }
	
	std::shared_ptr<FTextureBinding> Binding = std::make_shared<FTextureBinding>();

	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	TextureBindings.Add(_Name, Binding);

	return Binding;
}

std::shared_ptr<FSamplerBinding> FRenderResourceCollection::SetSamplerBinding(const FString& _Name, int _BindPoint,
                                                                              bool bIsUseVertexShader, bool bIsUsePixelShader)
{
	std::shared_ptr< FSampler> Res = FSampler::Find(_Name);

	// if (Res == nullptr)
	// {
	// 	//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
	// 	Res = FConstantBuffer::Create(_Name, _DataSize);
	// }
	
	std::shared_ptr<FSamplerBinding> Binding = std::make_shared<FSamplerBinding>();

	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	SamplerBindings.Add(_Name, Binding);

	return Binding;
}

std::shared_ptr<FConstantBufferBinding> FRenderResourceCollection::SetConstantBufferBinding(const FString& _Name,
                                                                                            const void* _CPUDataPtr, int _DataSize, int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader)
{

	/*std::shared_ptr<class FConstantBufferBinding>* Binding = ConstantBufferBindings.Find(_Name);

	if()*/
	std::shared_ptr<FConstantBuffer> Res = FConstantBuffer::Find(_Name);

	if (Res == nullptr)
	{
		//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
		Res = FConstantBuffer::Create(_Name, _DataSize);
	}
	
	std::shared_ptr<FConstantBufferBinding> Binding = std::make_shared<FConstantBufferBinding>();

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

