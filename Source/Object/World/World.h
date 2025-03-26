#pragma once
#include "Core/Container/Array.h"
#include "Core/Container/Set.h"
#include "Core/Math/Vector.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/ObjectMacros.h"
#include "Core/Utils/JsonSavehelper.h"
#include "Debug/DebugConsole.h"
#include "Object/ObjectFactory.h"


class URenderer;
class AActor;

class UPrimitiveComponent;

class UWorld :public UObject
{
	DECLARE_CLASS(UWorld, UObject)

public:
	UWorld() = default;

public:
	void InitWorld();

	void BeginPlay();
	void Tick(float DeltaTime);
	void LateTick(float DeltaTime);

	void OnDestroy();

	template <typename T>
		requires std::derived_from<T, AActor>
	T* SpawnActor();
  
	bool DestroyActor(AActor* InActor);
	
	void Render(uint32 ViewportIndex);
	void RenderPickingTexture(URenderer& Renderer);
	//void DisplayPickingTexture(URenderer& Renderer);
	void RenderMainTexture(URenderer& Renderer);

	void ClearWorld();
	void ClearWorldWithoutCamera();
	void LoadWorld(const char* InSceneName);
	void SaveWorld();

	void AddZIgnoreComponent(UPrimitiveComponent* InComponent);
	void RemoveZIgnoreComponent(UPrimitiveComponent* InComponent) {ZIgnoreRenderComponents.Remove(InComponent); }
	bool ContainsZIgnoreComponent(UPrimitiveComponent* InComponent) {return ZIgnoreRenderComponents.Find(InComponent) != -1; }
	
	// render
	void AddRenderComponent(UPrimitiveComponent* Component) { RenderComponents.Add(Component); }
	void RemoveRenderComponent(UPrimitiveComponent* Component) { RenderComponents.Remove(Component); }

	inline ACamera* GetCamera() const { return Camera; }
	inline TMap<EOrthoViewMode, ACamera*> GetCameraList() const { return CameraList; }

	void SetCamera(ACamera* NewCamera) { Camera = NewCamera; }
	inline void AddCamera(ACamera* NewCamera) { CameraList.Add(NewCamera->GetOrthoViewType(), NewCamera); }
	void ClearCameraList() { CameraList.Empty(); }

	void RayCasting(const FVector& MouseNDCPos);

	void PickByPixel(const FVector& MousePos);

	TArray<AActor*>& GetActors() { return Actors; }

	TArray<FString> ExcludedClasses;
	
	TArray<AActor*>& GetDisplayedActors();

	float& GetGridSizePtr() { return GridSize; }

	void OnChangedGridSize();

	float GetGridSize() const { return GridSize; }
private:
	UWorldInfo GetWorldInfo() const;
	ACamera* Camera = nullptr;
	TMap<EOrthoViewMode, ACamera*> CameraList;

	float GridSize = 100.0f;

public:
	FString SceneName;
	uint32 Version = 1;
	
protected:
	TArray<AActor*> Actors;
	TArray<AActor*> DisplayedActors;
	TArray<UPrimitiveComponent*> ZIgnoreRenderComponents;
	TArray<AActor*> ActorsToSpawn;
	TArray<AActor*> PendingDestroyActors; // TODO: 추후에 TQueue로 변경
	TSet<UPrimitiveComponent*> RenderComponents;

// Editor Only
public:
	//TArray<class ULayer*> Layers;

	TArray<AActor*> ActiveGroupActors;
// End Editor Only
public:
	//class ULevel* Level = nullptr;
	//class ULineBatchComponent* LineBatcher;

	//TArray<FVector> ViewLocationRenderedLastFrame;
	//TArray<FWorldCachedViewInfo> CachedViewInfoRenderedLastFrame;

	//class AGameModeBase* GameMode = nullptr;
	//class AGameStateBase* GameState = nullptr;
	//class UGameInstance* GameInstance = nullptr;

private:
	//TArray<class AController> Controllers;
	//TArray<class APlayerController> PlayerControllers;
	//TArray<class ACameraActor> CameraActors;

	//class FTimeManger* TimeManager = nullptr;

// Editor Only
public:
	//class FSceneInterface* Scene = nullptr;
	//TArray<FLevelViewportInfo> EditorViews;
// End Editor Only


};

template <typename T>
	requires std::derived_from<T, AActor>
T* UWorld::SpawnActor()
{
	T* Actor = FObjectFactory::ConstructObject<T>();
	
	if (UWorld* World = UEngine::Get().GetWorld())
	{
		Actor->SetWorld(World);
		Actors.Add(Actor);
		ActorsToSpawn.Add(Actor);
		return Actor;
	}

	UE_LOG("Actor Construction Failed. World is nullptr");
	return nullptr;
}