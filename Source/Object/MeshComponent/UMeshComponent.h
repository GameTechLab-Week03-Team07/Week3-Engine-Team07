#pragma once
#include "Core/Container/String.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "SimpleJSON/Json.hpp" 

// 메시 컴포넌트 기본 클래스
class UMeshComponent : public UPrimitiveComponent
{
public:
	UMeshComponent();
	virtual ~UMeshComponent();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// 메시 컴포넌트 공통 기능
	virtual void SetMeshMaterial(const std::string& MaterialName);
	virtual void UpdateMeshBounds();

	// 렌더링 관련 함수
	virtual void UpdateMeshConstantBuffer();

protected:
	// 메시 렌더링 관련 속성
	bool bUseCustomMaterial;
	std::string MaterialOverride;

	// 메시 바운드 계산 관련
	virtual FBoxSphereBounds CalcMeshBounds(const FTransform& LocalToWorld) const;

	// 렌더링 리소스 초기화
	virtual void InitializeRenderResources();
};