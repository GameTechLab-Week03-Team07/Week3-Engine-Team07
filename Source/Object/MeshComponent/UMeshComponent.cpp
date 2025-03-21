#include "UMeshComponent.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Object/Actor/Camera.h"
#include "Static/FEditorManager.h" 

UMeshComponent::UMeshComponent()
	: bUseCustomMaterial(false)
{
	// 메시 컴포넌트 기본 초기화
}

UMeshComponent::~UMeshComponent()
{
	// 리소스 정리
}

void UMeshComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeRenderResources();
}

void UMeshComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateMeshConstantBuffer();
}

void UMeshComponent::SetMeshMaterial(const std::string& MaterialName)
{
	bUseCustomMaterial = true;
	MaterialOverride = MaterialName;

	// 렌더링 리소스 컬렉션에 재질 설정
	SetMaterial(MaterialName);
}

void UMeshComponent::UpdateMeshBounds()
{
	UpdateBounds();
}

void UMeshComponent::UpdateMeshConstantBuffer()
{
	// 상수 버퍼 업데이트 로직
	FMatrix ModelMatrix;
	CalculateModelMatrix(ModelMatrix);

	const FMatrix& ViewProjectionMatrix = UEngine::Get().GetWorld()->GetCamera()->GetViewProjectionMatrix();
	FMatrix MVP = FMatrix::Transpose(ModelMatrix * ViewProjectionMatrix);

	FConstantsComponentData& Data = GetConstantsComponentData();
	Data.MVP = MVP;
	Data.Color = GetCustomColor();
	Data.UUIDColor = FEditorManager::EncodeUUID(GetUUID());
	Data.bUseVertexColor = IsUseVertexColor();
}

FBoxSphereBounds UMeshComponent::CalcMeshBounds(const FTransform& LocalToWorld) const
{
	// 기본 구현은 BoxExtent 사용
	return FBoxSphereBounds(FBox(-BoxExtent, BoxExtent)).TransformBy(LocalToWorld);
}

void UMeshComponent::InitializeRenderResources()
{
	// 기본 렌더링 리소스 초기화
}