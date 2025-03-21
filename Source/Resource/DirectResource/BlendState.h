#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "Resource/Resource.h"
#include "Core/Container/String.h"


class FBlendState: public FResource<FBlendState> 
{
public:

	FBlendState();
	virtual ~FBlendState() override;
	
	static std::shared_ptr<FBlendState> Create(const FString& _Name, const D3D11_BLEND_DESC& _Desc)
	{
		std::shared_ptr<FBlendState> Res = CreateRes(_Name);
		Res->ResCreate(_Desc);
		return Res;
	}

	void Setting();

protected:
	void ResCreate(const D3D11_BLEND_DESC& _Desc);

private:
	D3D11_BLEND_DESC Desc = {};
	ID3D11BlendState* State = nullptr;
	//FVector4 Factor;
	UINT Mask = 0xffffffff;
};
