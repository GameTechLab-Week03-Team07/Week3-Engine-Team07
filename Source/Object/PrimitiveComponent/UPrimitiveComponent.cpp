#include "UPrimitiveComponent.h"

#include "Core/Rendering/URenderer.h"
#include "Debug/EngineShowFlags.h"
#include "Object/Actor/Actor.h"
#include "Object/Actor/Camera.h"
#include "Object/World/World.h"
#include "Resource/Mesh.h"
#include "Resource/DirectResource/BlendState.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Static/FEditorManager.h"


UPrimitiveComponent::UPrimitiveComponent()
{
	bCanBeRendered = true;


	// 기본으로 바인딩되는 데이타
	GetRenderResourceCollection().SetConstantBufferBinding("FConstantsComponentData", &ConstantsComponentData, 0, true, true);
	SetMaterial("DefaultMaterial");
}

UPrimitiveComponent::~UPrimitiveComponent()
{
}

void UPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPrimitiveComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime); 
	UpdateBounds();
}

// void UPrimitiveComponent::UpdateConstantPicking(const URenderer& Renderer, const FVector4 UUIDColor)const
// {
// 	Renderer.UpdateConstantPicking(UUIDColor);
// }

// void UPrimitiveComponent::UpdateConstantDepth(const URenderer& Renderer, const int Depth)const
// {
// 	Renderer.UpdateConstantDepth(Depth);
// }

void UPrimitiveComponent::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();
	if (Renderer == nullptr || !FEngineShowFlags::Get().GetSingleFlag(EEngineShowFlags::SF_Primitives) || !bCanBeRendered)
	{
		return;
	}
	// if (GetOwner()->Implements<IGizmoInterface>() == false) // TODO: RTTI 개선하면 사용
	if (!dynamic_cast<IGizmoInterface*>(GetOwner()))
	{
		if (bIsPicked)
		{
			bUseVertexColor = false;
			SetCustomColor(FVector4(1.0f, 0.647f, 0.0f, 1.0f));
		}
		else
		{
			bUseVertexColor = true;
		}
	}

	FMatrix ModelMatrix;
	CalculateModelMatrix(ModelMatrix);

	const FMatrix& ViewProjectionMatrix = UEngine::Get().GetWorld()->GetCamera()->GetViewProjectionMatrix();

	FMatrix MVP = FMatrix::Transpose(
		ModelMatrix *
		ViewProjectionMatrix
);
	
	uint32 ID = GetUUID();

	FVector4 UUIDCOlor = FEditorManager::EncodeUUID(ID);

	FConstantsComponentData& Data = GetConstantsComponentData();

	Data = {
		.MVP = MVP,
		.Color = GetCustomColor(),
		.UUIDColor = UUIDCOlor,
		.bUseVertexColor = IsUseVertexColor()
	};
	

	Renderer->Render(GetRenderResourceCollection());
}

void UPrimitiveComponent::CalculateModelMatrix(FMatrix& OutMatrix)
{
	//빌보드 행렬계산
	if (bIsBillboard == true)
	{
		ACamera* cam = UEngine::Get().GetWorld()->GetCamera();

		FVector cameraPosition = cam->GetActorTransform().GetPosition();


		FVector objectPosition = GetWorldTransform().GetPosition();	
		FVector objectScale = GetWorldTransform().GetScale();

		FVector lookDir = (cameraPosition - objectPosition).GetSafeNormal();

		// 언리얼 좌표계에 맞춘 구형 빌보드, Z축이 상방 벡터
		// Y축(우측)은 상방 벡터와 시선 방향의 외적
		FVector right = FVector(0, 0, 1).Cross(lookDir).GetSafeNormal();

		FVector up = lookDir.Cross(right).GetSafeNormal();

		// X축(전방)은 우측 벡터와 상방 벡터의 외적
		// 언리얼에서는 X가 전방이므로 이렇게 계산
		//FVector forward = right.Cross(up).GetSafeNormal();

		FMatrix rotationMatrix;

		// 회전 행렬 구성
		rotationMatrix.X = FVector4(lookDir.X, lookDir.Y, lookDir.Z, 0);
		rotationMatrix.Y = FVector4(right.X, right.Y, right.Z, 0);
		rotationMatrix.Z = FVector4(up.X, up.Y, up.Z, 0);
		rotationMatrix.W = FVector4(0, 0, 0, 1);

		FMatrix positionMatrix = FMatrix::GetTranslateMatrix(objectPosition);
		FMatrix scaleMatrix = FMatrix::GetScaleMatrix(objectScale);

		OutMatrix = scaleMatrix * rotationMatrix * positionMatrix;

		return;
	}
	OutMatrix = GetWorldTransform().GetMatrix();
	return;
}

void UPrimitiveComponent::RegisterComponentWithWorld(UWorld* World)
{
	World->AddRenderComponent(this);
}

void UPrimitiveComponent::SetBoundsScale(float NewBoudnsScale)
{
	BoundsScale = NewBoudnsScale;
	UpdateBounds();
}

void UPrimitiveComponent::UpdateBounds()
{
	FBoxSphereBounds OriginalBounds = Bounds;
	Super::UpdateBounds();
}

void UPrimitiveComponent::SetBoxExtent(const FVector& InExtent)
{
	BoxExtent = InExtent;
	UpdateBounds();
}

FBoxSphereBounds UPrimitiveComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FBox(-BoxExtent, BoxExtent)).TransformBy(LocalToWorld);
}

UCubeComp::UCubeComp()
{
	SetMesh("Cube");
	bCanBeRendered = true;
	Max = GetMesh()->GetVertexBuffer()->GetMax();
	Min = GetMesh()->GetVertexBuffer()->GetMin();

	FVector extent = (Max - Min) / 2;
	SetBoxExtent(extent);
}

USphereComp::USphereComp()
{
	SetMesh("Sphere");
	bCanBeRendered = true;
	Max = GetMesh()->GetVertexBuffer()->GetMax();
	Min = GetMesh()->GetVertexBuffer()->GetMin();

	FVector extent = (Max - Min) / 2;
	float radius = extent.Length();

	SetSphereRadius(radius);
}

void USphereComp::SetSphereRadius(float InSphereRadius)
{
	Radius = InSphereRadius;
	UpdateBounds();
}

FBoxSphereBounds USphereComp::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(FVector::ZeroVector, FVector(Radius, Radius, Radius), Radius).TransformBy(LocalToWorld);
}

float USphereComp::GetShapeScale() const
{
	FTransform LocalToWorld = GetComponentTransform();

	// Scale3DAbsXYZ1 = { Abs(X), Abs(Y)), Abs(Z), 0 }
	FVector4 ScaleAbsXYYZ0 = FVector4(FMath::Abs(LocalToWorld.GetScale().X), FMath::Abs(LocalToWorld.GetScale().Y), FMath::Abs(LocalToWorld.GetScale().Z), 0.0f);
	// Scale3DAbsYZX1 = { Abs(Y),Abs(Z)),Abs(X), 0 }
	FVector4 ScaledAbsYZX0 = FVector4(ScaleAbsXYYZ0.Y, ScaleAbsXYYZ0.Z, ScaleAbsXYYZ0.X, 0.0f);
	// Scale3DAbsZXY1 = { Abs(Z),Abs(X)),Abs(Y), 0 }
	FVector4 ScaledAbsZXY0 = FVector4(ScaleAbsXYYZ0.Z, ScaleAbsXYYZ0.X, ScaleAbsXYYZ0.Y, 0.0f);

	// t0 = { Min(Abs(X), Abs(Y)),  Min(Abs(Y), Abs(Z)), Min(Abs(Z), Abs(X)), 0 }
	FVector4 t0 = FVector4(FMath::Min(ScaleAbsXYYZ0.X, ScaledAbsYZX0.X), FMath::Min(ScaleAbsXYYZ0.Y, ScaleAbsXYYZ0.Y), FMath::Min(ScaleAbsXYYZ0.Z, ScaleAbsXYYZ0.Z), 0.0f);
	// t1 = { Min(Abs(X), Abs(Y), Abs(Z)), Min(Abs(Y), Abs(Z), Abs(X)), Min(Abs(Z), Abs(X), Abs(Y)), 0 }
	FVector4 t2 = FVector4(FMath::Max(t0.X, ScaledAbsZXY0.X), FMath::Max(t0.Y, ScaledAbsZXY0.Y), FMath::Max(t0.Z, ScaledAbsZXY0.Z), 0.0f);
	// Scale3DAbsMax = Min(Abs(X), Abs(Y), Abs(Z));
	float scaleAbsMin = FMath::Min(t2.X, FMath::Min(t2.Y, t2.Z));

	return scaleAbsMin;
}

UTriangleComp::UTriangleComp()
{
	SetMesh("Triangle");
	bCanBeRendered = true;
}

UQuadComp::UQuadComp()
{
	SetMesh("Quad");
	bCanBeRendered = true;
}

ULineComp::ULineComp()
{
	SetMesh("Line");
}

UCylinderComp::UCylinderComp()
{
	SetMesh("Cylinder");
}

UConeComp::UConeComp()
{
	SetMesh("Cone");
}
