#include "RenderResourceCollection.h"

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
	//Material->Setting(); 

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
	
	// 섹션 정보가 있으면 섹션별로 처리
	if (Mesh->GetSections().Num() != 0)
	{
		// Mesh 내부에 저장된 머티리얼 정보를 가져옵니다.
		//const TMap<std::string, FObjMaterialInfo>& materialMap = Mesh->GetMaterials();

		// 각 섹션마다 처리
		for (const auto& Section : Mesh->GetSections())
		{
			// 섹션의 SlotName(예: "RedMaterial", "BlueMaterial")을 이용해 머티리얼 객체를 찾습니다.
			std::shared_ptr<FMaterial> sectionMat = FMaterial::Find(Section.SlotName);
			if (!sectionMat)
			{
				// 해당 머티리얼이 없다면 기본 머티리얼("StaticMeshMaterial")을 사용합니다.
				sectionMat = FMaterial::Find("StaticMeshMaterial");
			}


			MatIndexData.MatIndex = Section.MaterialIndex;



			for (auto& Binding : ConstantBufferBindings)
			{
				Binding.Value->Setting();
			}

			// 머티리얼 정보를 머티리얼 파라미터에 업데이트합니다.
			//auto it = materialMap.Find(Section.SlotName);
			//if (it != nullptr)
			//{
			//	const FObjMaterialInfo& matInfo = *it;
			//	sectionMat->SetDiffuseColor(matInfo.DiffuseColor);
			//	if (!matInfo.DiffuseTexture.empty())
			//	{
			//		sectionMat->SetDiffuseTexture(matInfo.DiffuseTexture);
			//		// 필요 시 텍스처 바인딩 로직을 추가하세요.
			//	}
			//}
			// 
			// 업데이트된 머티리얼 상태를 적용합니다.
			sectionMat->Setting();
			// 해당 섹션에 대해 DrawIndexed 호출합니다.
			FDevice::Get().GetDeviceContext()->DrawIndexed(Section.indexCount, Section.indexStart, 0);
		}
	}
	else
	{
		// 섹션 정보가 없는 경우 기존 방식
		Material->Setting();
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

void FRenderResourceCollection::UpdateMatIndexConstantBuffer(int MatIndex)
{
	MatIndexData.MatIndex = MatIndex;
	// 정수형 머티리얼 인덱스를 GPU로 전달
	SetConstantBufferBinding(
		"MatIndexConstantBuffer",                       // 바인딩 이름
		&MatIndexData,                     // CPU에서 보낼 데이터 포인터
		sizeof(FMatIndexConstantsComponentData),                        // 데이터 크기
		3,                                 // 바인딩 포인트 (예시)
		true,                               // Vertex Shader에 사용
		true                                // Pixel Shader에 사용
	);
}

