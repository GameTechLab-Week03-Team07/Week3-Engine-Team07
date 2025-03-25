#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "Resource/Resource.h"
#include "Core/Container/String.h"


class FRasterizer:
public FResource<FRasterizer> 
{
public:
	// constrcuter destructer
	FRasterizer();
	~FRasterizer();

	static std::shared_ptr<FRasterizer> Create(const FString& _Name, const D3D11_RASTERIZER_DESC& _Desc)
	{
		std::shared_ptr<FRasterizer> Res = FRasterizer::CreateRes(_Name);
		Res->ResCreate(_Desc);
		return Res;
	}

	void Setting(bool bOverrideRasterizer);

protected:
	void ResCreate(const D3D11_RASTERIZER_DESC& _Desc);


private:
	D3D11_RASTERIZER_DESC Desc = {};
	ID3D11RasterizerState* State = nullptr;
};
