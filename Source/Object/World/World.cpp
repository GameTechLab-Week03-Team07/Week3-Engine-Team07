#include "World.h"
#include <cassert>
#include "Core/Utils/JsonSavehelper.h"

#include "Core/Container/Map.h"
#include "Core/Input/PlayerInput.h"
#include "Object/Actor/Camera.h"
#include <Object/Gizmo/GizmoHandle.h>

#include "Object/Actor/Cone.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Actor/Sphere.h"
#include "Object/Actor/StaticMesh.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"
#include "Static/FLineBatchManager.h"
#include "Static/FUUIDBillBoard.h"
#include <Core/Math/Ray.h>

#include "Core/Rendering/URenderer.h"
#include "Object/Actor/Arrow.h"
#include "Object/Actor/Picker.h"
#include "Core/Config/ConfigManager.h"
#include "Object/Gizmo/GizmoActor.h"
#include "Object/MeshComponent/UStaticMeshComponent.h"
#include "Resource/Mesh.h"

#include "Debug/DebugDrawManager.h"


void UWorld::InitWorld()
{
	//TODO : 
	GridSize = FString::ToFloat(UConfigManager::Get().GetValue(TEXT("World"), TEXT("GridSize")));
}

void UWorld::BeginPlay()
{
	for (const auto& Actor : Actors)
	{
		Actor->BeginPlay();
	}

	APlayerInput::Get().RegisterMouseDownCallback(EKeyCode::LButton, [this](const FVector& MouseNDCPos)
	{
		RayCasting(MouseNDCPos);
	}, GetUUID());
}

void UWorld::Tick(float DeltaTime)
{
	for (const auto& Actor : ActorsToSpawn)
	{
		Actor->BeginPlay();
	}
	ActorsToSpawn.Empty();

	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->Tick(DeltaTime);
		}
	}
}

void UWorld::LateTick(float DeltaTime)
{
	const auto CopyActors = Actors;
	for (const auto& Actor : CopyActors)
	{
		if (Actor->CanEverTick())
		{
			Actor->LateTick(DeltaTime);
		}
	}

	for (const auto& PendingActor : PendingDestroyActors)
	{
		// Engine에서 제거
		UEngine::Get().GObjects.Remove(PendingActor->GetUUID());
	}
	PendingDestroyActors.Empty();
}

void UWorld::OnDestroy()
{
	UConfigManager::Get().SaveConfig("editor.ini");
}

void UWorld::Render()
{
	URenderer* Renderer = UEngine::Get().GetRenderer();

	//라인 렌더링 임시



	if (Renderer == nullptr)
	{
		return;
	}

	ACamera* cam = FEditorManager::Get().GetCamera();
	cam->UpdateCameraMatrix();


	//if (APlayerInput::Get().GetKeyDown(EKeyCode::LButton))
	//{
	//	RenderPickingTexture(*Renderer);
	//}

	RenderMainTexture(*Renderer);

	FLineBatchManager::Get().Render();

	AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr)
	{
		const FVector LocalMax = SelectedActor->GetActorLocalBoundsMax();
		const FVector LocalMin = SelectedActor->GetActorLocalBoundsMin();

		[[maybe_unused]] FVector WorldMax = SelectedActor->GetActorWorldBoundsMax();
		[[maybe_unused]] FVector WorldMin = SelectedActor->GetActorWorldBoundsMin();

		UDebugDrawManager::Get().DrawBoundingBox(LocalMin, LocalMax, SelectedActor->GetActorTransform(), FVector4::RED);
	}
	UDebugDrawManager::Get().Render();


	FLineBatchManager::Get().Render();

	FUUIDBillBoard::Get().Render();


	//DisplayPickingTexture(*Renderer);

}

void UWorld::RenderPickingTexture(URenderer& Renderer)
{
	// Renderer.PreparePicking();
	// Renderer.PreparePickingShader();

	for (auto& RenderComponent : RenderComponents)
	{
		if (RenderComponent->GetOwner()->GetDepth() > 0)
		{
			continue;
		}
		// uint32 UUID = RenderComponent->GetUUID();
		// RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
		RenderComponent->Render();
	}

	// Renderer.PrepareZIgnore();
	for (auto& RenderComponent: ZIgnoreRenderComponents)
	{
		
		RenderComponent->Render();
		//MsgBoxAssert("없어진 기능입니다");
		// uint32 UUID = RenderComponent->GetUUID();
		// RenderComponent->UpdateConstantPicking(Renderer, APicker::EncodeUUID(UUID));
		// uint32 depth = RenderComponent->GetOwner()->GetDepth();
		// RenderComponent->Render();
	}
}

void UWorld::RenderMainTexture(URenderer& Renderer)
{
	// Renderer.Prepare();
	// Renderer.PrepareShader();
	// Renderer.PrepareMain();

	//Renderer.PrepareMainShader();
	for (auto& RenderComponent : RenderComponents)
	{
		if (RenderComponent->GetOwner()->GetDepth() > 0)
		{
			continue;
		}
		uint32 depth = RenderComponent->GetOwner()->GetDepth();
		// RenderComponent->UpdateConstantDepth(Renderer, depth);
		RenderComponent->Render();
	}

	FDevice::Get().PickingPrepare();

	//Renderer.PrepareZIgnore();
	for (auto& RenderComponent: ZIgnoreRenderComponents)
	{
		uint32 depth = RenderComponent->GetOwner()->GetDepth();
		RenderComponent->Render();
	}

	FDevice::Get().SetRenderTarget();
}

// void UWorld::DisplayPickingTexture(URenderer& Renderer)
// {
// 	Renderer.RenderPickingTexture();
// }

void UWorld::ClearWorld()
{
	TArray CopyActors = Actors;
	for (AActor* Actor : CopyActors)
	{
		// if (!Actor->Implements<IGizmoInterface>()) // TODO: RTTI 개선하면 사용
		if (!dynamic_cast<IGizmoInterface*>(Actor))
		{
			DestroyActor(Actor);
		}
	}

	UE_LOG("Clear World");
}


bool UWorld::DestroyActor(AActor* InActor)
{
	// 나중에 Destroy가 실패할 일이 있다면 return false; 하기
	assert(InActor);

	if (InActor->GetWorld() == nullptr)
	{
		return false;
	}

	if (PendingDestroyActors.Find(InActor) != -1)
	{
		return true;
	}

	// 삭제될 때 Destroyed 호출
	InActor->Destroyed();

	// World에서 제거
	Actors.Remove(InActor);

	// 제거 대기열에 추가
	PendingDestroyActors.Add(InActor);
	return true;
}

void UWorld::SaveWorld()
{
	JsonSaveHelper::SaveScene(GetWorldInfo());
}

void UWorld::AddZIgnoreComponent(UPrimitiveComponent* InComponent)
{
	ZIgnoreRenderComponents.Add(InComponent);
	//InComponent->SetIsOrthoGraphic(true);
}

void UWorld::LoadWorld(const char* InSceneName)
{
	if (InSceneName == nullptr || strcmp(InSceneName, "") == 0){
		return;
	}

	const std::unique_ptr<UWorldInfo> WorldInfo = JsonSaveHelper::LoadScene(InSceneName);
	if (WorldInfo == nullptr) return;

	ClearWorld();

	Version = WorldInfo->Version;
	this->SceneName = WorldInfo->SceneName;

	// Type 확인
	while (!WorldInfo->ObjectInfos.empty())
	{
		const std::unique_ptr<UObjectInfo> ObjectInfo = std::move(WorldInfo->ObjectInfos.front());
		WorldInfo->ObjectInfos.pop();

		FTransform Transform = FTransform(ObjectInfo->Location, FQuat(), ObjectInfo->Scale);
		Transform.Rotate(ObjectInfo->Rotation);

		AActor* Actor = nullptr;
		
		if (ObjectInfo->ObjectType == "Actor")
		{
			Actor = SpawnActor<AActor>();
		}
		else if (ObjectInfo->ObjectType == "Sphere")
		{
			Actor = SpawnActor<ASphere>();
		}
		else if (ObjectInfo->ObjectType == "Cube")
		{
			Actor = SpawnActor<ACube>();
		}
		else if (ObjectInfo->ObjectType == "Arrow")
		{
			Actor = SpawnActor<AArrow>();
		}
		else if (ObjectInfo->ObjectType == "Cylinder")
		{
			Actor = SpawnActor<ACylinder>();
		}
		else if (ObjectInfo->ObjectType == "Cone")
		{
			Actor = SpawnActor<ACone>();
		}
		else if (ObjectInfo->ObjectType == "StaticMesh")
		{
			// AStaticMesh 액터 생성
			Actor = SpawnActor<AStaticMesh>();

			// UStaticMeshComponent 가져오기
			UStaticMesh* MeshComp = FStaticMeshManager::Get().LoadObjStaticMesh(ObjectInfo->StaticMeshAssetPath);
			if (MeshComp)
			{
				UStaticMeshComponent* CastedObject = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
				CastedObject->SetStaticMesh(MeshComp);
			}
		}
		
		Actor->SetActorTransform(Transform);
	}
}

void UWorld::RayCasting(const FVector& MouseNDCPos)
{
	FMatrix ProjMatrix = Camera->GetProjectionMatrix(); 
	FRay worldRay = FRay(Camera->GetViewMatrix(), ProjMatrix, MouseNDCPos.X, MouseNDCPos.Y);

	FLineBatchManager::Get().AddLine(worldRay.GetOrigin(), worldRay.GetOrigin() + worldRay.GetDirection() * Camera->GetFar(), FVector4::CYAN);

	AActor* SelectedActor = nullptr;
	float minDistance = FLT_MAX;

	for (auto& Actor : Actors)
	{
		UPrimitiveComponent* PrimitiveComponent = Actor->GetComponentByClass<UPrimitiveComponent>();
		if (PrimitiveComponent == nullptr)
		{
			continue;
		}

		FMatrix primWorldMat = PrimitiveComponent->GetComponentTransform().GetMatrix();
		FRay localRay = FRay::TransformRayToLocal(worldRay, primWorldMat.Inverse());

		std::shared_ptr<FMesh> CurMesh = PrimitiveComponent->GetMesh();
		CurMesh->GetVertexBuffer();

		float outT = 0.0f;
		bool bHit = false;

		switch (PrimitiveComponent->GetType())
		{
		case EPrimitiveType::EPT_Cube:
		{
			bHit = FRayCast::IntersectRayAABB(localRay, FVector(-0.5f, -0.5f, -0.5f), FVector(0.5f, 0.5f, 0.5f), outT);
			if (bHit)
			{
				UE_LOG("Cube Hit");
			}
			break;
		}
		case EPrimitiveType::EPT_Sphere:
		{
			bHit = FRayCast::InsertSectRaySphere(localRay, PrimitiveComponent->GetActorPosition(), 0.5f, outT);
			if (bHit)
			{
				UE_LOG("Sphere Hit");
			}
			break;
		}
		case EPrimitiveType::EPT_Cylinder:
		{
			bHit = FRayCast::IntersectRayAABB(localRay, FVector(0, -0.5f, 0), FVector(0, 0.5f, 0), outT);
			if (bHit)
			{
				UE_LOG("Cylinder Hit");
			}
			break;
		}
		case EPrimitiveType::EPT_Cone:
		{
			break;
		}
		case EPrimitiveType::EPT_Triangle:
		{
			break; 
		}
		default:
			break;
		}
		// 4. 교차가 발생했다면, 월드 좌표에서의 거리를 비교하여 최소 거리를 가진 액터 선택
		if (bHit)
		{
			float distance = worldRay.GetPoint(outT).Length();
			if (distance < minDistance)
			{
				minDistance = distance;
				SelectedActor = Actor;
				FUUIDBillBoard::Get().SetTarget(SelectedActor);
			}
		}
	}

	if (SelectedActor)
	{
		FEditorManager::Get().SelectActor(SelectedActor);
	}
}

void UWorld::PickByPixel(const FVector& MousePos)
{

}

void UWorld::OnChangedGridSize()
{
	UConfigManager::Get().SetValue(TEXT("World"), TEXT("GridSize"), FString::SanitizeFloat(GridSize));
	FLineBatchManager::Get().DrawWorldGrid(GridSize, GridSize/100.f);
}

UWorldInfo UWorld::GetWorldInfo() const
{
	UWorldInfo WorldInfo;
	WorldInfo.ActorCount = Actors.Num();
	WorldInfo.SceneName = *SceneName;
	WorldInfo.Version = 1;
	uint32 i = 0;
	for (auto& actor : Actors)
	{
		// if (actor->Implements<IGizmoInterface>()) // TODO: RTTI 개선하면 사용
		if (dynamic_cast<IGizmoInterface*>(actor))
		{
			WorldInfo.ActorCount--;
			continue;
		}
		auto ObjectInfo = std::make_unique<UObjectInfo>(
			UObjectInfo{
				.Location = actor->GetActorPosition(),
				.Rotation = actor->GetActorRotation(),
				.Scale = actor->GetActorScale(),
				.ObjectType = actor->GetTypeName(),
				.UUID = actor->GetUUID()
			}
		);

		// StaticMesh 타입인 경우 메시 경로 추가
		if (actor->GetTypeName() == "StaticMesh")
		{
			UStaticMeshComponent* CastedObject = Cast<UStaticMeshComponent>(actor->GetRootComponent());
			if (CastedObject->GetStaticMesh()) 
			{
				ObjectInfo->StaticMeshAssetPath = CastedObject->GetStaticMesh()->GetAssetPathFileName();
			}
		};

		WorldInfo.ObjectInfos.push(std::move(ObjectInfo));
		i++;
	}
	return WorldInfo;
}