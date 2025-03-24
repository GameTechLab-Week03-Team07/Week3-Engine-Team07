#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"


class FInputLayout:
public FResource<FInputLayout> 
{
public:
	FInputLayout() = default;
	~FInputLayout() override;

	static D3D11_INPUT_ELEMENT_DESC LayoutDesc[];
	//static D3D11_INPUT_ELEMENT_DESC LayoutTextureArrayDesc[];


	//인풋 레이아웃에는 INPUT_ELEMENT_DESC와 버텍스 쉐이더가 필요하나 일단 인풋 정보는 고정값
	static std::shared_ptr<FInputLayout> Create(const FString& _Name, std::shared_ptr<class FVertexShader> _Shader, bool bUseTextureArray = false)
	{
		std::shared_ptr<FInputLayout> Res = FInputLayout::CreateRes(_Name);
		Res->ResCreate(_Shader);
		return Res;
	}

	void ResCreate(
		std::shared_ptr<FVertexShader> _Shader
	);

	void Setting();
	
private:
	
	
	ID3D11InputLayout* LayOut = nullptr;
	//ID3D11InputLayout* LayOutTextureArray = nullptr;

};
