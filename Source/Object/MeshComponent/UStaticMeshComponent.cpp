#include "UStaticMeshComponent.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBUffer.h"
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
	// 정적메시 정보로 버퍼 생성
	for (auto section : StaticMesh->StaticMeshAsset->sections)
	{
		// 버퍼 생성
		FVertexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), section.vertices);
		FIndexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), section.Indices);

		// 머티리얼 이름 가져오기
		const std::string& materialName = section.MaterialSlotName;
		// 해당 머티리얼 찾기

		if (StaticMesh->StaticMeshAsset->Materials.Contains(materialName)) {
			const auto& material = StaticMesh->StaticMeshAsset->Materials[materialName];

			if (!material.DiffuseTexture.empty()) {
				FString textureSlotName = FString(materialName) + "_Diffuse";

				// 바인딩 시 섹션 또는 머티리얼에 따라 유니크하게 구분되도록
				GetRenderResourceCollection().SetTextureBinding(textureSlotName, /*BindSlot=*/2, false, true);
				GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", /*BindSlot=*/0, false, true);
			}
			// 5. 머티리얼 이름으로 렌더링 리소스에 설정
			GetRenderResourceCollection().SetMaterial(materialName);
		}
	}
}

FBoxSphereBounds UStaticMeshComponent::CalcMeshBounds(const FTransform& LocalToWorld) const
{
	//if (!StaticMesh || !StaticMesh->StaticMeshAsset || StaticMesh->StaticMeshAsset->Vertices.Num() == 0)
	{
		//return Super::CalcMeshBounds(LocalToWorld);
	}

	// 메시 정점을 기반으로 바운딩 박스 계산
	FVector Min(FLT_MAX, FLT_MAX, FLT_MAX);
	FVector Max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (auto section : StaticMesh->StaticMeshAsset->sections)
	{
		for (auto vertex : section.vertices)
		{
			Min.X = FMath::Min(Min.X, vertex.X);
			Min.Y = FMath::Min(Min.Y, vertex.Y);
			Min.Z = FMath::Min(Min.Z, vertex.Z);

			Max.X = FMath::Max(Max.X, vertex.X);
			Max.Y = FMath::Max(Max.Y, vertex.Y);
			Max.Z = FMath::Max(Max.Z, vertex.Z);
		}
	}

	FBox BoundingBox(Min, Max);
	return FBoxSphereBounds(BoundingBox).TransformBy(LocalToWorld);
}
