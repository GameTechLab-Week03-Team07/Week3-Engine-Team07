#include "UStaticMeshComponent.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/VertexBuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBUffer.h"
#include "Resource/Mesh.h"

UStaticMeshComponent::UStaticMeshComponent()
{
	// 정적 메시 컴포넌트 초기화
	StaticMesh = FStaticMeshManager::Get().GetStaticMesh("12140_Skull_v3.obj");
	// 메시가 있으면 렌더링 리소스 초기화
	if (StaticMesh)
	{
		InitializeRenderResources();
	}
}

UStaticMeshComponent::UStaticMeshComponent(std::string MeshName)
{
	StaticMesh = FStaticMeshManager::Get().GetStaticMesh(MeshName);
	// 메시가 있으면 렌더링 리소스 초기화
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

	// 렌더러를 통해 메시 렌더링
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
	if (StaticMesh && StaticMesh->StaticMeshAsset)
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
	
	FVertexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Vertices);
	FIndexBuffer::Create(FString(StaticMesh->GetAssetPathFileName()), StaticMesh->StaticMeshAsset->Indices);
	std::shared_ptr<FMesh> mesh = FMesh::Create(FString(StaticMesh->GetAssetPathFileName()));
	//FString textureName =  StaticMesh->StaticMeshAsset->Materials["12140_Skull_v3"]/.
	GetRenderResourceCollection().SetMesh(mesh);
	GetRenderResourceCollection().SetMaterial("StaticMeshMaterial");
	GetRenderResourceCollection().SetTextureBinding("12140_Skull_v3_Diffuse", 2, false, true);
	GetRenderResourceCollection().SetSamplerBinding("LinearSamplerState", 0, false, true);
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
