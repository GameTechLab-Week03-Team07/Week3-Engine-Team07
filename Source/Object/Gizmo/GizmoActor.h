#pragma once
#include "Core/Interfaces/GizmoInterface.h"
#include "Object/Actor/Actor.h"
#include "Object/PrimitiveComponent/GizmoComponent.h"


class AGizmoActor : public AActor, public IGizmoInterface
{
	DECLARE_CLASS(AGizmoActor, AActor)

public:
	AGizmoActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	//~ Begin IGizmoInterface
	virtual bool IsGizmo() override { return true; }
	//~ End IGizmoInterface

	void SetSelectedAxis(ESelectedAxis InAxis) { SelectedAxis = InAxis; }
	ESelectedAxis GetSelectedAxis() const { return SelectedAxis; }

	EGizmoType GetGizmoType() const { return GizmoType; }

	void SetScaleByDistance();
	void UpdateGizmoTransform(AActor* TargetActor);
	void DoTransform(FTransform& AT, FVector Result, AActor* Actor);

private:
	ESelectedAxis SelectedAxis = ESelectedAxis::None;
	EGizmoType GizmoType = EGizmoType::Translate;

	TMap<ESelectedAxis, UGizmoComponent*> GizmoComponents;
};

