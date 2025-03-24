#include "Texture.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"

#include "Debug/DebugConsole.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"

FTexture::FTexture()
{
}

FTexture::~FTexture()
{
	if (nullptr != DSV)
	{
		DSV->Release();
		DSV = nullptr;
	}

	if (nullptr != SRV)
	{
		SRV->Release();
		SRV = nullptr;
	}

	if (nullptr != RTV)
	{
		RTV->Release();
		RTV = nullptr;
	}

	if (nullptr != Texture2D)
	{
		Texture2D->Release();
	}
}

void FTexture::VSSetting(UINT InSlot)
{
	FDevice::Get().GetDeviceContext()->VSSetShaderResources(InSlot, 1, &SRV);
}

void FTexture::PSSetting(UINT InSlot)
{
	FDevice::Get().GetDeviceContext()->PSSetShaderResources(InSlot, 1, &SRV);
}

void FTexture::CSSetting(UINT InSlot)
{
	FDevice::Get().GetDeviceContext()->CSSetShaderResources(InSlot, 1, &SRV);
}

void FTexture::VSReset(UINT InSlot)
{
	ID3D11ShaderResourceView* ResetRes = nullptr;
	FDevice::Get().GetDeviceContext()->VSSetShaderResources(InSlot, 1, &ResetRes);
}
void FTexture::PSReset(UINT InSlot)
{
	ID3D11ShaderResourceView* ResetRes = nullptr;
	FDevice::Get().GetDeviceContext()->PSSetShaderResources(InSlot, 1, &ResetRes);
}

void FTexture::CSReset(UINT InSlot)
{
	ID3D11ShaderResourceView* ResetRes = nullptr;
	FDevice::Get().GetDeviceContext()->CSSetShaderResources(InSlot, 1, &ResetRes);
}


void FTexture::CreateRenderTargetView()
{
	if (nullptr != RTV)
	{
		return;
	}

	if (nullptr == Texture2D)
	{
		MsgBoxAssert("만들어지지 않은 텍스처로 랜더타겟뷰를 생성하려고 했습니다.");
		return;
	}

	HRESULT Result = FDevice::Get().GetDevice()->CreateRenderTargetView(Texture2D, nullptr, &RTV);

	if (S_OK != Result)
	{
		MsgBoxAssert("랜더타겟뷰 생성에 실패했습니다.");
		return;
	}
}

void FTexture::CreateShaderResourceView()
{
	if (nullptr != SRV)
	{
		return;
	}

	if (nullptr == Texture2D)
	{
		MsgBoxAssert("만들어지지 않은 텍스처로 쉐이더 리소스 뷰 생성하려고 했습니다.");
		return;
	}

	HRESULT Result;

	// 깊이버퍼용 쉐이더리소스뷰
	if (D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL & Desc.BindFlags)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		Result = FDevice::Get().GetDevice()->CreateShaderResourceView(Texture2D, &srvDesc, &SRV);
	}
	// 이미지를 수정할수 있는 권한을 '만든다'
	else
	{
		Result = FDevice::Get().GetDevice()->CreateShaderResourceView(Texture2D, nullptr, &SRV);
	}

	if (S_OK != Result)
	{
		MsgBoxAssert("쉐이더 리소스 뷰 생성에 실패했습니다.");
		return;
	}
}

void FTexture::CreateDepthStencilView()
{
	if (nullptr != DSV)
	{
		return;
	}

	if (nullptr == Texture2D)
	{
		MsgBoxAssert("만들어지지 않은 텍스처로 쉐이더 리소스 뷰 생성하려고 했습니다.");
		return;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	//dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	dsvDesc.Flags = 0; // new in D3D11
	
	HRESULT Result = FDevice::Get().GetDevice()->CreateDepthStencilView(Texture2D, &dsvDesc, &DSV);

	if (S_OK != Result)
	{
		MsgBoxAssert("쉐이더 리소스 뷰 생성에 실패했습니다.");
		return;
	}
}

void FTexture::ResLoad(const FString& InPath)
{
	std::string str = *InPath;
	std::wstring wstr(str.begin(), str.end());

	ID3D11Resource* Resource = nullptr;

	// 파일 확장자 확인
	std::wstring extension = wstr.substr(wstr.find_last_of(L"."));
	std::transform(extension.begin(), extension.end(), extension.begin(), ::towlower);

	HRESULT hr = S_OK;

	// DDS 파일인 경우
	if (extension == L".dds")
	{
		hr = DirectX::CreateDDSTextureFromFile(
			FDevice::Get().GetDevice(),
			FDevice::Get().GetDeviceContext(),
			wstr.c_str(),
			&Resource,
			&SRV
		);
	}
	// 다른 이미지 형식(WIC 지원 형식)인 경우
	else
	{
		hr = DirectX::CreateWICTextureFromFile(
			FDevice::Get().GetDevice(),
			FDevice::Get().GetDeviceContext(),
			wstr.c_str(),
			&Resource,
			&SRV
		);
	}

	if (FAILED(hr))
	{
		MsgBoxAssert("텍스처 로드에 실패했습니다.");
		return;
	}

	// ID3D11Texture2D 인터페이스 가져오기
	Resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&Texture2D));

	// 리소스 해제 (SRV와 Texture2D가 참조를 유지하므로 안전함)
	if (Resource)
	{
		Resource->Release();
		Resource = nullptr;
	}
}


void FTexture::ResCreate(ID3D11Texture2D* InRes)
{
	Texture2D = InRes;

	Texture2D->GetDesc(&Desc);

	CreateRenderTargetView();
}


void FTexture::ResCreate(const D3D11_TEXTURE2D_DESC& _Desc)
{
	Desc = _Desc;

	HRESULT result = FDevice::Get().GetDevice()->CreateTexture2D(&Desc, nullptr, &Texture2D);

	if (S_OK != result)
	{
		MsgBoxAssert("텍스쳐가 생성되지 못했습니다.");
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET & Desc.BindFlags)
	{
		CreateRenderTargetView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL & Desc.BindFlags)
	{
		CreateDepthStencilView();
	}

	if (D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE & Desc.BindFlags)
	{
		CreateShaderResourceView();
	}
}

