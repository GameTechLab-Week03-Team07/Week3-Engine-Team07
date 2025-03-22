#include "Camera.h"

#include "Core/Input/PlayerInput.h"
#include "Core/Config/ConfigManager.h"
#include "Static/FEditorManager.h"


ACamera::ACamera()
{
    Near = .1f;
    Far = 1000.f;
    FieldOfView = 45.f;
    ProjectionMode = ECameraProjectionMode::Perspective;
	CameraSpeed = 1.0f;
	Sensitivity = std::stof(UConfigManager::Get().GetValue("Camera", "Sensitivity").GetData());

    RootComponent = AddComponent<USceneComponent>();
    
    FTransform StartPos = GetActorTransform();
    StartPos.SetPosition(FVector(-5, 0, 0));
    SetActorTransform(StartPos);
}

void ACamera::BeginPlay()
{
	Super::BeginPlay();
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::W, [this] { MoveForward(); }, GetUUID());
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::S, [this] { MoveBackward(); }, GetUUID());
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::A, [this] { MoveLeft(); }, GetUUID());
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::D, [this] { MoveRight(); }, GetUUID());
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::Q, [this] { MoveDown(); }, GetUUID());
	APlayerInput::Get().RegisterKeyPressCallback(EKeyCode::E, [this] { MoveUp(); }, GetUUID());

	APlayerInput::Get().RegisterKeyDownCallback(EKeyCode::F, [this]
	{
		if (const AActor* SelectedActor = FEditorManager::Get().GetSelectedActor())
		{
			if (SelectedActor == this) return;
			SetActorPosition(SelectedActor->GetActorPosition() - (GetForward() * 10.0f));
		}
	}, GetUUID());

	APlayerInput::Get().RegisterMousePressCallback(EKeyCode::RButton, std::bind(&ACamera::Rotate, this, std::placeholders::_1), GetUUID());

	UConfigManager::Get().SetValue("Camera", "Sensitivity", std::to_string(Sensitivity));
}

void ACamera::SetFieldOfVew(float Fov)
{
    FieldOfView = Fov;
}  

void ACamera::SetFar(float Far)
{
    this->Far = Far;
}

void ACamera::SetNear(float Near)
{
    this->Near = Near;
}

float ACamera::GetFieldOfView() const
{
    return  FieldOfView;
}

float ACamera::GetNear() const
{
    return Near;
}

float ACamera::GetFar() const
{
    return Far;
}

void ACamera::InitMatrix()
{
	ViewMatrix = FMatrix::Identity();
	ProjectionMatrix = FMatrix::Identity();
	ViewProjectionMatrix = FMatrix::Identity();
}

void ACamera::UpdateCameraMatrix(float AspectRatio)
{
	//뷰 매트릭스 업데이트
	ViewMatrix = GetActorTransform().GetViewMatrix();
	
	// 프로젝션 매트릭스 업데이트

	float FOV = FMath::DegreesToRadians(GetFieldOfView());
	float Near = GetNear();
	float Far = GetFar();

	if (ProjectionMode == ECameraProjectionMode::Perspective)
	{
		ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);
	}
	else if (ProjectionMode == ECameraProjectionMode::Orthographic)
	{
		ProjectionMatrix = FMatrix::OrthoForLH(UEngine::Get().GetScreenWidth() / ZoomSize, UEngine::Get().GetScreenHeight() / ZoomSize, Near, Far);

		// TODO: 추가 필요.
		// ProjectionMatrix = FMatrix::OrthoForLH(FOV, AspectRatio, Near, Far);
	}

	ViewProjectionMatrix = ViewMatrix * ProjectionMatrix ;
}

void ACamera::MoveForward()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() + (GetForward() * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::MoveBackward()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() - (GetForward() * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::MoveLeft()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() - (GetRight() * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::MoveRight()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() + (GetRight() * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::MoveUp()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() + (FVector::UpVector * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::MoveDown()
{
	FTransform tr = GetActorTransform();
	tr.SetPosition(tr.GetPosition() - (FVector::UpVector * CameraSpeed * UEngine::GetDeltaTime()));
	SetActorTransform(tr);
}

void ACamera::Rotate(const FVector& mouseDelta)
{
	FTransform tr = GetActorTransform();
	FVector TargetRotation = tr.GetRotation().GetEuler();
	TargetRotation.Y -= FMath::Clamp(Sensitivity * mouseDelta.Y, -MaxYDegree, MaxYDegree);
	TargetRotation.Z += Sensitivity * mouseDelta.X;
	tr.SetRotation(TargetRotation);

	SetActorTransform(tr);
	//TargetRotation.Y = FMath::Clamp(TargetRotation.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
	//CameraTransform.SetRotation(TargetRotation);


	//float CamSpeed = Camera->CameraSpeed;

	////회전이랑 마우스클릭 구현 카메라로 해야할듯?
	//CameraTransform.Translate(NewVelocity * DeltaTime * CamSpeed);
	//Camera->SetActorTransform(CameraTransform); //임시용
}
