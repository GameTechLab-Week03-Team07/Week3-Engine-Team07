#pragma once
#include <memory>

#include "Core/AbstractClass/Singleton.h"
#include "Core/Math/Vector.h"
#include "Object/Actor/Camera.h"

class AActor;
class AGizmoActor;

class FEditorManager : public TSingleton<FEditorManager>
{
	friend class URenderer;
	friend class FDevice;;
public:
	void Init();
    
    inline AActor* GetSelectedActor() const {return SelectedActor;}
    
    void SelectActor(AActor* NewActor);

    inline ACamera* GetCamera() const {return Camera;}
	inline TMap<EOrthoViewMode, ACamera*> GetCameraList() const { return CameraList; }

	void SetCamera(ACamera* NewCamera) { Camera = NewCamera; }
	inline void AddCamera(ACamera* NewCamera) { CameraList.Add(NewCamera->GetOrthoViewType(), NewCamera); }
	inline void ClearCameraList() { CameraList.Empty(); }

	inline AGizmoActor* GetGizmo() const {return Gizmo;}

	static FVector4 EncodeUUID(uint32 UUID);
	static uint32 DecodeUUID(FVector4 color);

	void LateTick(float DeltaTime);

	void OnUpdateWindowSize(uint32 Width, uint32 Height);

	void OnResizeComplete();

	FVector4 GetPixel(FVector MPos) const;
    
private:
    ACamera* Camera = nullptr;
	TMap<EOrthoViewMode, ACamera*> CameraList;

    AActor* SelectedActor = nullptr;
	AGizmoActor* Gizmo = nullptr;

	

	std::shared_ptr<class FTexture> UUIDTexture = nullptr;
	//std::shared_ptr<class FTexture> UUIDTextureDepthStecil = nullptr;
};




