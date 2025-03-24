#include "InputLayout.h"
#include "Resource/DirectResource/VertexShader.h"
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"


D3D11_INPUT_ELEMENT_DESC FInputLayout::LayoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// 이걸 추가로 작성할 필요가 있을까?
//D3D11_INPUT_ELEMENT_DESC FInputLayout::LayoutTextureArrayDesc[] = {
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
//};

FInputLayout::~FInputLayout()
{
	if (nullptr != LayOut)
	{
		LayOut->Release();
		LayOut = nullptr;
	}
}


void FInputLayout::ResCreate(std::shared_ptr<FVertexShader> _Shader)
{
	//const std::vector<D3D11_INPUT_ELEMENT_DESC>& Infos = _Buffer->VertexInfoPtr->Infos;

	FDevice::Get().GetDevice()->CreateInputLayout(
		&LayoutDesc[0],
		ARRAYSIZE(LayoutDesc),
		_Shader->BinaryCode->GetBufferPointer(),
		_Shader->BinaryCode->GetBufferSize(),
		&LayOut);

	if (LayOut == nullptr) MsgBoxAssert("Error: FInputLayout Create Failed");
}

void FInputLayout::Setting()
{
	if (LayOut == nullptr) MsgBoxAssert("Error: FInputLayout Create Failed in LayOut Setting");

	// 버텍스버퍼를 여러개 넣어줄수 있다.
	FDevice::Get().GetDeviceContext()->IASetInputLayout(LayOut);
	return;
}
