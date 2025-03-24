#include "Cylinder.h"

#include "Core/Input/PlayerInput.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

ACylinder::ACylinder()
{
    bCanEverTick = true;

    UCylinderComp* CylinderComponent = AddComponent<UCylinderComp>();

    SetActorTransform(FTransform());
}

void ACylinder::BeginPlay()
{
    Super::BeginPlay();
}

void ACylinder::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
	
	FTransform NewTransform = GetActorTransform();
	FVector NewPosition = NewTransform.GetPosition();
    [[maybe_unused]] FVector NewRotation = NewTransform.GetRotation().GetEuler();
	FVector NewScale = NewTransform.GetScale();

	if (APlayerInput::Get().GetKeyDown(EKeyCode::Up))
	{
		NewPosition += FVector(0.0f, 0.0f, 1.0f);
	}
	if (APlayerInput::Get().GetKeyDown(EKeyCode::Down))
	{
		NewPosition += FVector(0.0f, 0.0f, -1.0f);
	}
	if (APlayerInput::Get().GetKeyDown(EKeyCode::Left))
	{
		NewPosition += FVector(0.0f, 1.0f, 0.0f);
	}    
	if (APlayerInput::Get().GetKeyDown(EKeyCode::Right))
	{
		NewPosition += FVector(0.0f, -1.0f, 0.0f);
	}

	if (APlayerInput::Get().GetKeyDown(EKeyCode::J))
	{
		NewScale += FVector(0.1f, 0.0f, 0.0f);
	}
	if (APlayerInput::Get().GetKeyDown(EKeyCode::K))
	{
		NewScale += FVector(0.0f, 0.1f, 0.0f);
	}
	if (APlayerInput::Get().GetKeyDown(EKeyCode::L))
	{
		NewScale += FVector(0.0f, 0.0f, 0.1f);
	}


	if (APlayerInput::Get().GetKeyPress(EKeyCode::Z))
	{
		NewTransform.Rotate(FVector(1, 0, 0));
	}
	if (APlayerInput::Get().GetKeyPress(EKeyCode::X))
	{
		NewTransform.Rotate(FVector(0, 1, 0));
	}
	if (APlayerInput::Get().GetKeyPress(EKeyCode::C))
	{
		NewTransform.Rotate(FVector(0, 0, 1));
	}

	NewTransform.SetPosition(NewPosition);
	NewTransform.SetScale(NewScale);
	SetActorTransform(NewTransform);
}

const char* ACylinder::GetTypeName()
{
    return "Cylinder";
}
