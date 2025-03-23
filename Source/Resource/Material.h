#pragma once
#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Resource/DirectResource/Vertexbuffer.h"


class FDepthStencilState;
class FRasterizer;
class FBlendState;

class FMaterial : public FResource<FMaterial>
{
public:
	FMaterial();
	virtual ~FMaterial() override;

	static std::shared_ptr<FMaterial> Create(const FString& InName)
	{
		std::shared_ptr<FMaterial> NewRes = CreateRes(InName);
		return NewRes;
	}

	void VertexShader();
	void Rasterizer();
	void PixelShader();
	void Blend();
	void DepthStencil();


	void SetVertexShader(const FString& InValue);
	void SetRasterizer(const FString& InValue);
	void SetPixelShader(const FString& InValue);
	void SetBlendState(const FString& InValue);
	void SetDepthState(const FString& InValue);

	std::shared_ptr<class FVertexShader> GetVertexShader()
	{
		return VertexShaderPtr;
	}

	std::shared_ptr<class FPixelShader> GetPixelShader()
	{
		return PixelShaderPtr;
	}

	void SetDiffuseColor(const FVector& InColor) { DiffuseColor = InColor; }
	const FVector& GetDiffuseColor() const { return DiffuseColor; }

	void SetDiffuseTexture(const FString& InTextureName) { DiffuseTextureName = InTextureName; }
	const FString& GetDiffuseTexture() const { return DiffuseTextureName; }

	void Setting();
	
private:
	std::shared_ptr<FVertexShader> VertexShaderPtr = nullptr;
	std::shared_ptr<FPixelShader> PixelShaderPtr = nullptr;
	std::shared_ptr<FRasterizer> RasterizerPtr = nullptr;
	std::shared_ptr<FBlendState> BlendStatePtr = nullptr;
	std::shared_ptr<FDepthStencilState> DepthStencilPtr = nullptr;

	FVector DiffuseColor = FVector(1.0f, 1.0f, 1.0f);
	FString DiffuseTextureName;
};



