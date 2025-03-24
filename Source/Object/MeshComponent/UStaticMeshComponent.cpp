#include "UStaticMeshComponent.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Core/Rendering/FDevice.h"

UStaticMeshComponent::UStaticMeshComponent()
{
	// 현재 딱히 할 거 없음
	if (StaticMesh)
	{
		InitializeRenderResources();
	}
}

UStaticMeshComponent::~UStaticMeshComponent()
{
	// 리소스 정리
}

void UStaticMeshComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStaticMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//void UStaticMeshComponent::Render()
//{
//	if (!StaticMesh || !StaticMesh->StaticMeshAsset || !bCanBeRendered)
//	{
//		return;
//	}
//
//	// 상수 버퍼 업데이트
//	UpdateMeshConstantBuffer();
//
//	// 메시 렌더링
//	GetRenderResourceCollection().Render();
//}

//void UStaticMeshComponent::Render()
//{
//	// 렌더링 가능한 상태 확인
//	if (!StaticMesh || !StaticMesh->StaticMeshAsset || !bCanBeRendered)
//	{
//		return;
//	}
//
//	// 공통 상수 버퍼 업데이트 (MVP, Color 등)
//	UpdateMeshConstantBuffer();
//
//	// 렌더 리소스 컬렉션에서 설정한 Mesh와 Material을 가져옵니다.
//	FRenderResourceCollection& renderCollection = GetRenderResourceCollection();
//
//	// 버텍스/인덱스 버퍼, 입력 레이아웃 등 Mesh 관련 설정을 한 번 수행
//	renderCollection.GetMesh()->Setting();
//	// Material 설정 (셰이더, 상태 등)
//	renderCollection.GetMaterial()->Setting();
//
//	// 각 섹션마다 MaterialIndex를 상수 버퍼에 업데이트하고 DrawIndexed 호출
//	for (const auto& section : renderCollection.GetMesh()->Sections)
//	{
//		// 섹션에 해당하는 머티리얼 인덱스를 설정할 상수 데이터 준비
//		FConstantStaticData constantData;
//		constantData.MaterialIndex = section.MaterialIndex;
//		constantData.Padding = FVector(0, 0, 0);
//
//		UpdateStaticConstantBuffer(staticData);
//
//		// 상수 버퍼 업데이트: "StaticConstantBuffer"라는 이름의 상수 버퍼를 사용한다고 가정
//		// BindPoint는 해당 상수 버퍼가 바인딩될 슬롯 번호 (예: 픽셀 셰이더 b3 등)를 지정합니다.
//		auto constantBinding = renderCollection.SetConstantBufferBinding("MaterialConstants",
//			&constantData,
//			sizeof(FConstantStaticData),
//			/*BindPoint*/3,
//			false,  // bIsUseVertexShader
//			true);  // bIsUsePixelShader
//		constantBinding->Setting();
//
//		// 섹션의 인덱스 범위에 대해 DrawIndexed 호출
//		FDevice::Get().GetDeviceContext()->DrawIndexed(section.indexCount, section.indexStart, 0);
//	}
//}

void UStaticMeshComponent::Render()
{
	if (!StaticMesh || !StaticMesh->StaticMeshAsset || !bCanBeRendered)
	{
		return;
	}

	// 공통 상수 버퍼 업데이트 (MVP, Color 등)
	UpdateMeshConstantBuffer();

	FRenderResourceCollection& renderCollection = GetRenderResourceCollection();

	// Mesh와 Material 설정
	renderCollection.GetMesh()->Setting();
	renderCollection.GetMaterial()->Setting();

	// 각 서브메시(섹션)마다 MaterialIndex 업데이트 후 DrawIndexed 호출
	for (const auto& section : renderCollection.GetMesh()->Sections)
	{
		FConstantStaticData staticData;
		staticData.MaterialIndex = section.MaterialIndex;
		//staticData.Padding = FVector(0, 0, 0); // 필요에 따라 설정

		// 상수 버퍼 업데이트 함수 호출
		UpdateStaticConstantBuffer(staticData);
		GetRenderResourceCollection().Render(section);
	}
}


void UStaticMeshComponent::UpdateStaticConstantBuffer(const FConstantStaticData& InData)
{
	// 렌더 리소스 컬렉션을 통해 "StaticConstantBuffer"에 접근하여 상수 데이터를 업데이트합니다.
	auto constantBinding = GetRenderResourceCollection().SetConstantBufferBinding(
		"MaterialConstants",     // 미리 생성된 상수 버퍼 이름
		&InData,                    // 업데이트할 데이터의 포인터
		sizeof(FConstantStaticData),// 데이터 크기
		/*BindPoint*/ 3,            // 셰이더에서 b3에 바인딩되었다고 가정
		false,                      // Vertex Shader에 바인딩할 필요 없으므로 false
		true                        // Pixel Shader에 바인딩 (MaterialConstants는 Pixel Shader에서 사용)
	);
	constantBinding->Setting(); // 내부적으로 ChangeData를 호출하여 상수 버퍼를 업데이트합니다.
}

void UStaticMeshComponent::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	if (StaticMesh != InStaticMesh)
	{
		StaticMesh = InStaticMesh;

		if (StaticMesh)
		{
			UpdateMeshBounds();
			InitializeRenderResources();
		}
	}
}

FBoxSphereBounds UStaticMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	//if (StaticMesh && StaticMesh->StaticMeshAsset)
		if (StaticMesh)
	{
		return CalcMeshBounds(LocalToWorld);
	}

	return Super::CalcBounds(LocalToWorld);
}

void UStaticMeshComponent::InitializeRenderResources()
{
	if (!StaticMesh || !StaticMesh->StaticMeshAsset)
	{
		return;
	}

	// 1. 버텍스/인덱스 버퍼 생성 (기존 방식)
	FVertexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Vertices);
	FIndexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Indices);

	// 2. FMesh 생성 및 섹션 정보 복사
	std::shared_ptr<FMesh> mesh = FMesh::Create(FString(StaticMesh->GetAssetPathFileName()));

	// FStaticMeshAsset에 저장된 섹션 정보를 FMesh의 Sections 멤버에 복사합니다.
	mesh->Sections.Empty();
	for (const auto& section : StaticMesh->StaticMeshAsset->Sections)
	{
		mesh->Sections.Add(section);
	}
	// 3. 렌더 리소스 컬렉션에 Mesh와 Material 설정
	GetRenderResourceCollection().SetMesh(mesh);
	GetRenderResourceCollection().SetMaterial("StaticMeshMaterial"); // 전혀 상관 없지 않나?

	// 4. 머티리얼에 따른 텍스처 바인딩 처리
	if (StaticMesh->StaticMeshAsset->MaterialInfoArray.Len() != 0)
	{
		for (const auto& pair : StaticMesh->StaticMeshAsset->MaterialSlotNameToIndex)
		{
			const std::string& MaterialName = pair.Key;
			int matIndex = pair.Value;

			if (matIndex < 0 || matIndex >= StaticMesh->StaticMeshAsset->MaterialInfoArray.Len())
				continue;

			const auto& Material = StaticMesh->StaticMeshAsset->MaterialInfoArray[matIndex];
			if (!Material.DiffuseTexture.empty()) {
				FString TextureName = FString(MaterialName) + "_Diffuse";
				GetRenderResourceCollection().SetTextureBinding(TextureName, 2, false, true);
				GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", 0, false, true);
			}
		}
	}
}

FBoxSphereBounds UStaticMeshComponent::CalcMeshBounds(const FTransform& LocalToWorld) const
{
	if (!StaticMesh || !StaticMesh->StaticMeshAsset || StaticMesh->StaticMeshAsset->Vertices.Num() == 0)
	{
		//return Super::CalcMeshBounds(LocalToWorld);
	}

	// 메시 정점을 기반으로 바운딩 박스 계산
	FVector Min(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (const FVertexSimple& Vertex : StaticMesh->StaticMeshAsset->Vertices)
	{
		Min.X = FMath::Min(Min.X, Vertex.X);
		Min.Y = FMath::Min(Min.Y, Vertex.Y);
		Min.Z = FMath::Min(Min.Z, Vertex.Z);

		Max.X = FMath::Max(Max.X, Vertex.X);
		Max.Y = FMath::Max(Max.Y, Vertex.Y);
		Max.Z = FMath::Max(Max.Z, Vertex.Z);
	}

	FBox BoundingBox(Min, Max);
	return FBoxSphereBounds(BoundingBox).TransformBy(LocalToWorld);
}
