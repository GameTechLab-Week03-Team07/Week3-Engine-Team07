#include "Core/Container/String.h"
#include "Object/MeshComponent/UMeshComponent.h"
#include "Resource/StaticMesh/UStaticMesh.h"
#include "Resource/StaticMesh/FStaticMeshManager.h"

// 정적 메시 컴포넌트 클래스
class UStaticMeshComponent : public UMeshComponent
{
public:
	UStaticMeshComponent();
	UStaticMeshComponent(std::string MeshName);
	virtual ~UStaticMeshComponent();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Render() override;

	// 메시 설정 함수
	void SetStaticMesh(UStaticMesh* InStaticMesh);
	UStaticMesh* GetStaticMesh() const { return StaticMesh; }

	// 직렬화 함수
	void Serialize(bool bIsLoading, json::JSON& Handle)
	{
		//Super::Serialize(bIsLoading, Handle);

		if (bIsLoading)
		{
			std::string assetName = Handle["ObjStaticMeshAsset"].ToString();
			StaticMesh = FStaticMeshManager::Get().LoadObjStaticMesh(assetName);

			if (StaticMesh)
			{
				UpdateMeshBounds();
				InitializeRenderResources();
			}
		}
		else
		{
			FString assetName = StaticMesh ? StaticMesh->GetAssetPathFileName() : "None";
			Handle["ObjStaticMeshAsset"] = *assetName;
		}
	}

	// 바운드 계산 오버라이드
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

protected:
	// 정적 메시 참조
	UStaticMesh* StaticMesh;

	// 렌더링 리소스
	std::shared_ptr<FVertexBuffer> VertexBuffer;
	std::shared_ptr<FIndexBuffer> IndexBuffer;

	// 렌더링 리소스 초기화 오버라이드
	virtual void InitializeRenderResources() override;

	// 메시 데이터로부터 바운드 계산
	virtual FBoxSphereBounds CalcMeshBounds(const FTransform& LocalToWorld) const override;
};
