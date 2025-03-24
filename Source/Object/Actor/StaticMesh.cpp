#include "StaticMesh.h"
#include "Object/MeshComponent/UStaticMeshComponent.h"

AStaticMesh::AStaticMesh()
{
	bCanEverTick = true;

	UStaticMeshComponent* StaticMeshComponent = AddComponent<UStaticMeshComponent>();
	RootComponent = StaticMeshComponent;

	StaticMeshComponent->SetRelativeTransform(FTransform());
}

void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AStaticMesh::GetTypeName()
{
	return "StaticMesh";
}
