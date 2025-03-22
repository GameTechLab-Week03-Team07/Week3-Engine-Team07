#include "PlayerController.h"

#include "PlayerInput.h"
#include "Core/Math/Quat.h"
#include "Object/Actor/Camera.h"
#include "Static/FEditorManager.h"


void APlayerController::HandleCameraMovement(float DeltaTime) const
{
    FVector NewVelocity(0, 0, 0);

    if (APlayerInput::Get().GetKeyPress(EKeyCode::RButton) == false)
    {
       // Camera->SetVelocity(NewVelocity);
		return;
    }

    ACamera* Camera = FEditorManager::Get().GetCamera();
    
    //전프레임이랑 비교
    //x좌표 받아와서 x만큼 x축회전
    //y좌표 받아와서 y만큼 y축 회전
    FVector DeltaPos = APlayerInput::Get().GetMouseDeltaPos();
    //FQuat CameraRot = FEditorManager::Get().GetCamera()->GetActorTransform().GetRotation();

    FTransform CameraTransform = Camera->GetActorTransform();

    FVector TargetRotation = CameraTransform.GetRotation().GetEuler();
    TargetRotation.Y -= FMath::Clamp(Camera->Sensitivity * DeltaPos.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
    TargetRotation.Z += Camera->Sensitivity * DeltaPos.X;
    CameraTransform.SetRotation(TargetRotation);

    
    float CamSpeed = Camera->CameraSpeed;

    //회전이랑 마우스클릭 구현 카메라로 해야할듯?
    CameraTransform.Translate(NewVelocity * DeltaTime * CamSpeed);
    Camera->SetActorTransform(CameraTransform); //임시용
    // FCamera::Get().SetVelocity(NewVelocity);
}

void APlayerController::HandleGizmoMovement(float DeltaTime) const
{
    if (APlayerInput::Get().GetKeyDown(EKeyCode::LButton) == false)
    {
        return;
    }

    AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
    
    if (SelectedActor == nullptr) //이거를 나중엔 기즈모로 체크
    {
        return;
    }

    
}

//void APlayerController::DrawHUDs()
//{
//	canvas.Begin();
//	for (auto hud : statHUDs) {
//		hud->SetCanvas(&canvas);
//		hud->DrawHUD();
//	}
//}
//
void APlayerController::ProcessPlayerInput(float DeltaTime) const
{
	HandleGizmoMovement(DeltaTime);
	// HandleCameraMovement(DeltaTime);
}
