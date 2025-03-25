#include "Rasterizer.h"
#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"
#include "Core/Rendering/FViewMode.h"


FRasterizer::FRasterizer()
{
}

FRasterizer::~FRasterizer()
{
	if (nullptr != State)
	{
		State->Release();
		State = nullptr;
	}
}

void FRasterizer::Setting(bool bOverrideRasterizer)
{

	EViewModeIndex Index = FViewMode::Get().GetViewMode();


	if (Index == EViewModeIndex::VMI_Default || !bOverrideRasterizer)
	{
		if (nullptr == State)
		{
			UE_LOG("Error: FRasterizer Setting Failed");
		}
		FDevice::Get().GetDeviceContext()->RSSetState(State);
	}
	else
	{
		FViewMode::Get().ApplyViewMode();
	}
}

void FRasterizer::ResCreate(const D3D11_RASTERIZER_DESC& _Desc)
{
	Desc = _Desc;
	if (S_OK != FDevice::Get().GetDevice()->CreateRasterizerState(&_Desc, &State))
	{
		UE_LOG("Error: FRasterizer Setting Failed");
	}
}
