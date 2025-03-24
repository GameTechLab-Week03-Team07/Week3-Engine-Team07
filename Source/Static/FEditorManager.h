#pragma once
#include <memory>

#include "Core/AbstractClass/Singleton.h"
#include "Core/Math/Vector.h"

class ACamera;
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


    void SetCamera(ACamera* NewCamera);

	AGizmoActor* GetGizmo() const {return Gizmo;}

	static FVector4 EncodeUUID(uint32 UUID);
	static uint32 DecodeUUID(FVector4 color);

	void LateTick(float DeltaTime);

	void OnUpdateWindowSize(uint32 Width, uint32 Height);

	void OnResizeComplete();

	FVector4 GetPixel(FVector MPos) const;
    
private:
    ACamera* Camera = nullptr;

    AActor* SelectedActor = nullptr;
	AGizmoActor* Gizmo = nullptr;

	

	std::shared_ptr<class FTexture> UUIDTexture = nullptr;
	//std::shared_ptr<class FTexture> UUIDTextureDepthStecil = nullptr;
};




