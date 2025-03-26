#pragma once

#define _TCHAR_DEFINED
#include <d3dcommon.h>

#include "Core/Engine.h"
#include "Core/Math/Matrix.h"
#include "Object/USceneComponent.h"
#include "Primitive/PrimitiveVertices.h"
#include "Resource/RenderResourceCollection.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include <Resource/Material.h>
#include <Resource/Mesh.h>
#include "Resource/ShaderConstants.h"

class FVertexShader;
class FRasterizer;
class FDepthStencilState;
class FBlendState;
class FPixelShader;
class FConstantBufferBinding;
class FConstantBuffer;
class FInputLayout;

class UPrimitiveComponent : public USceneComponent
{
	DECLARE_CLASS(UPrimitiveComponent, USceneComponent)

public:
	UPrimitiveComponent();
	virtual ~UPrimitiveComponent() override;

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	//void UpdateConstantPicking(const URenderer& Renderer, FVector4 UUIDColor) const;
	//void UpdateConstantDepth(const URenderer& Renderer, int Depth) const;
	virtual void Render();
	virtual void CalculateModelMatrix(FMatrix& OutMatrix);

	virtual EPrimitiveType GetType() { return EPrimitiveType::EPT_None; }

	bool IsUseVertexColor() const { return bUseVertexColor; }

	void SetCustomColor(const FVector4& InColor)
	{
		CustomColor = InColor; 
		bUseVertexColor = false;
	}

	void SetUseVertexColor(bool bUse)
	{
		bUseVertexColor = bUse;
	}
	const FVector4& GetCustomColor() const { return CustomColor; }

public:
	virtual void RegisterComponentWithWorld(UWorld* World);

public:
	void SetCanBeRendered(bool bRender) { bCanBeRendered = bRender; }

	void SetIsOrthoGraphic(bool IsOrtho) { bIsOrthoGraphic = IsOrtho; }
	bool GetIsOrthoGraphic() const { return bIsOrthoGraphic;}

	void SetIsPicking(bool IsPicking) { bIsPicking = IsPicking; }
	bool GetIsPicking() const { return bIsPicking; }
	FConstantsComponentData& GetConstantsComponentData() { return ConstantsComponentData; }
	//void SetConstantsComponentData(FConstantsComponentData& ) { bIsBillboard = bBillboard; }

	std::shared_ptr<FMesh> GetMesh() const { return RenderResourceCollection.GetMesh(); }
	std::shared_ptr<FMaterial> GetMaterial() const { return RenderResourceCollection.GetMaterial(); }

	void SetMesh(const FString& InName) { RenderResourceCollection.SetMesh(InName); }
	void SetMaterial(const FString& InName) { RenderResourceCollection.SetMaterial(InName); }
	
	FRenderResourceCollection& GetRenderResourceCollection() { return RenderResourceCollection; }
public:
	void SetBoundsScale(float NewBoudnsScale);

	virtual void UpdateBounds() override;
protected:
	float BoundsScale = 1.0f;
protected:
	bool bCanBeRendered = false;
	bool bIsBillboard = false;
	bool bUseVertexColor = true;
	bool bIsOrthoGraphic = false;
	bool bIsPicking = false;

	FVector4 CustomColor = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

protected:
	FRenderResourceCollection RenderResourceCollection;
	FConstantsComponentData ConstantsComponentData;

public:
	void SetBoxExtent(const FVector& InExtent);

	inline FVector GetScaledBoxExtent() const { return BoxExtent * GetComponentTransform().GetScale(); }

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	inline void InitBoxExtent(const FVector& InExtent) { BoxExtent = InExtent; }

public:
	FVector BoxExtent;
};

class UCubeComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UCubeComp, UPrimitiveComponent)

public:
	UCubeComp();

	virtual EPrimitiveType GetType() override
	{

		return EPrimitiveType::EPT_Cube;
	}
};

class USphereComp : public UPrimitiveComponent
{
	DECLARE_CLASS(USphereComp, UPrimitiveComponent)

public:
	USphereComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Sphere;
	}

	/**
	* Change the sphere radius. This is the unscaled radius, before component scale is applied.
	* @param	InSphereRadius: the new sphere radius
	*/
	void SetSphereRadius(float InSphereRadius);

	// @return the radius of the sphere, with component scale applied.
	inline float GetScaledSphereRadius() const { return Radius * GetShapeScale(); }

	// @return the radius of the sphere, ignoring component scale.
	inline float GetUnscaledSphereRadius() const { return Radius; }

	//~ Begin UPrimitiveComponent Interface.
	virtual inline bool IsZeroExtent() { return Radius == 0.0f; }
	//virtual struct FCollisionShape GetCollisionShape(float Inflation = 0.0f) const override;
	//~ End UPrimitiveComponent Interface.

	//~ Begin USceneComponent Interface
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End USceneComponent Interface

	// Get the scale used by this shape. This is a uniform scale that is the minimum of any non-uniform scaling.
	// @return the scale used by this shape.
	float GetShapeScale() const;

	// Sets the sphere radius without triggering a render or physics update.
	inline void InitSphereRadius(float InSphereRadius) { Radius = InSphereRadius; }

protected:
	float Radius;
};

class UTriangleComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UTriangleComp, UPrimitiveComponent)

public:
	UTriangleComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Triangle;
	}
};

class ULineComp : public UPrimitiveComponent
{
	DECLARE_CLASS(ULineComp, UPrimitiveComponent)


public:
	ULineComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Line;
	}
};

class UCylinderComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UCylinderComp, UPrimitiveComponent)


public:
	UCylinderComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Cylinder;
	}
};

class UConeComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UConeComp, UPrimitiveComponent)

public:
	UConeComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Cone;
	}
};

class UQuadComp : public UPrimitiveComponent
{
	DECLARE_CLASS(UQuadComp, UPrimitiveComponent)

public:
	UQuadComp();

	virtual EPrimitiveType GetType() override
	{
		return EPrimitiveType::EPT_Quad;
	}
};