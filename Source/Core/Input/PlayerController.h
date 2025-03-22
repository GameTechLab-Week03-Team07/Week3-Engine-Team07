#pragma once
#include "Core/AbstractClass/Singleton.h"


class APlayerController : public TSingleton<APlayerController>
{
public:
    APlayerController() = default;

    void ProcessPlayerInput(float DeltaTime) const;

    void HandleCameraMovement(float DeltaTime) const;
    void HandleGizmoMovement(float DeltaTime) const;
    void HandleSplitterMovement(float DeltaTime) const;
};
