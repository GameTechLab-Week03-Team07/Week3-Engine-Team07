#include "UStaticMeshComponent.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/Mesh.h"

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

void UStaticMeshComponent::Render()
{
	if (!StaticMesh || !StaticMesh->StaticMeshAsset || !bCanBeRendered)
	{
		return;
	}

	// 상수 버퍼 업데이트
	UpdateMeshConstantBuffer();

	// 메시 렌더링
	GetRenderResourceCollection().Render();
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

	// 정적 메시 데이터로 버퍼 생성
	FString AssetName(StaticMesh->GetAssetPathFileName());
	FVertexBuffer::Create(AssetName, StaticMesh->StaticMeshAsset->Vertices);
	FIndexBuffer::Create(AssetName, StaticMesh->StaticMeshAsset->Indices);

	// FMesh 객체 생성 및 섹션 설정
	std::shared_ptr<FMesh> mesh = FMesh::Create(AssetName);
	if (StaticMesh->StaticMeshAsset->MaterialInfoArray.Len() != 0)
	{
		mesh->SetSections(StaticMesh->StaticMeshAsset->Sections);
	}

	// 렌더 리소스 컬렉션에 메시와 머티리얼 설정
	GetRenderResourceCollection().SetMesh(mesh);
	GetRenderResourceCollection().SetMaterial("StaticMeshMaterial");

	// MaterialInfoArray가 있을 경우 텍스처 및 샘플러 바인딩 처리
	if (StaticMesh->StaticMeshAsset->Sections.Num() != 0)
	{
		// Diffuse 텍스처 바인딩 (register t2)
		FString diffuseTexArrayName = "StaticMeshTextureArray_" + AssetName;
		GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", 0, false, true);
		GetRenderResourceCollection().SetTextureBinding(diffuseTexArrayName, 2, false, true);

		// Normal 텍스처 바인딩 (register t3)
		FString normalTexArrayName = "StaticMeshNormalTextureArray_" + AssetName;
		GetRenderResourceCollection().SetTextureBinding(normalTexArrayName, 3, false, true);

		// Specular 텍스처 바인딩 (register t4)
		FString specularTexArrayName = "StaticMeshSpecularTextureArray_" + AssetName;
		GetRenderResourceCollection().SetTextureBinding(specularTexArrayName, 4, false, true);
	}

	UpdateMatIndexConstantBuffer();
}

FBoxSphereBounds UStaticMeshComponent::CalcMeshBounds(const FTransform& LocalToWorld) const
{
	if (!StaticMesh || !StaticMesh->StaticMeshAsset || StaticMesh->StaticMeshAsset->Vertices.Num() == 0)
	{
		return Super::CalcMeshBounds(LocalToWorld);
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

void UStaticMeshComponent::UpdateMatIndexConstantBuffer()
{
	// 정수형 머티리얼 인덱스를 GPU로 전달
	GetRenderResourceCollection().SetConstantBufferBinding(
		"MatIndexConstantBuffer",                       // 바인딩 이름
		&GetRenderResourceCollection().MatIndexData,                     // CPU에서 보낼 데이터 포인터
		sizeof(FMatIndexConstantsComponentData),                        // 데이터 크기
		3,                                 // 바인딩 포인트 (예시)
		true,                               // Vertex Shader에 사용
		true                                // Pixel Shader에 사용
	);
}
