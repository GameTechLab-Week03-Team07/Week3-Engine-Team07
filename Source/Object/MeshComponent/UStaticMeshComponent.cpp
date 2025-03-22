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
	FVertexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Vertices);
	FIndexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Indices);
	std::shared_ptr<FMesh> mesh = FMesh::Create(FString(StaticMesh->GetAssetPathFileName()));
	FString textureName = StaticMesh->StaticMeshAsset->Materials["12140_Skull_v3"].PathFileName;
	GetRenderResourceCollection().SetMesh(mesh);
	GetRenderResourceCollection().SetMaterial("StaticMeshMaterial");
	
	if (!StaticMesh->StaticMeshAsset->Materials.IsEmpty())
	{
		// 첫 번째 머티리얼 사용 (또는 다른 선택 로직 구현)
		for (const auto& MaterialPair : StaticMesh->StaticMeshAsset->Materials)
		{
			const auto& MaterialName = MaterialPair.Key;
			const auto& Material = MaterialPair.Value;

			// 텍스처 경로가 있는지 확인
			if (!Material.DiffuseTexture.empty())
			{
				// 텍스처 이름 생성 (MaterialName + "_Diffuse")
				FString TextureName = FString(MaterialName) + "_Diffuse";

				// 텍스처 및 샘플러 바인딩
				GetRenderResourceCollection().SetTextureBinding(TextureName, 2, false, true);
				GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", 0, false, true);

				// 첫 번째 유효한 텍스처만 사용하려면 여기서 break
				break;
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
