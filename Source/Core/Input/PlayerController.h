#pragma once
#include "Core/AbstractClass/Singleton.h"


class APlayerController : public TSingleton<APlayerController>
{
public:
    APlayerController() = default;

    void ProcessPlayerInput(float DeltaTime) const;

    void HandleCameraMovement(float DeltaTime) const;
	void HandleGizmoMovement(float DeltaTime) const;

	//void DrawHUDs();

private:
	// FIXME : 언리얼에서 FCanvas는 매 프레임마다 UGameViewportCliet::Draw()에서 생성되지만
	// 현재 프로젝트에 해당 구조가 명확하지 않아 우선 PlayerController에서 대체.
	/*FCanvas canvas;
	TMap <FName, AHUD*> StatHUDs;*/
	//AHUD* FPSHUD;
	//AHUD* MemoryHUD;
};
