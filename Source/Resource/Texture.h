#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"


class FTexture : public FResource<FTexture> 
{
public:

	// constructor & destructor
	FTexture();
	virtual ~FTexture() override;

	// static std::shared_ptr<FTexture> Load(std::string_view _Path, 
	// D3D11_FILTER _Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
	// D3D11_TEXTURE_ADDRESS_MODE _Address = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP)
	// {
	// 	return Load(Path.GetStringPath(), Path.GetFileName(), _Filter, _Address);
	//}

	static std::shared_ptr<FTexture> Load(const FString& InPath, const FString& InName)
		// D3D11_FILTER _Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		// D3D11_TEXTURE_ADDRESS_MODE _Address = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP)
	{
		std::shared_ptr<FTexture> NewRes = CreateRes(InName);
		NewRes->ResLoad(InPath);
		return NewRes;
	}
	
	//리소스 등록
	static std::shared_ptr<FTexture> Create(const FString& InName, ID3D11Texture2D* InRes)
	{
		std::shared_ptr<FTexture> NewRes = CreateRes(InName);
		NewRes->ResCreate(InRes);
		return NewRes;
	}
	
	//텍스쳐 생성
	static std::shared_ptr<FTexture> Create(const FString& InName, const D3D11_TEXTURE2D_DESC& InDesc)
	{
		std::shared_ptr<FTexture> NewRes = CreateRes(InName);
		NewRes->ResCreate(InDesc);
		return NewRes;
	}

	void VSSetting(UINT InSlot);
	void PSSetting(UINT InSlot);
	void CSSetting(UINT InSlot);

	void VSReset(UINT InSlot);
	void PSReset(UINT InSlot);
	void CSReset(UINT InSlot);

	// 랜더타겟 세팅용
	void CreateRenderTargetView();
	// 쉐이더 세팅용
	void CreateShaderResourceView();
	// 깊버거 세팅용
	void CreateDepthStencilView();

	ID3D11Texture2D* GetTexture2D() const 
	{
		return Texture2D;
	}

	ID3D11RenderTargetView* GetRTV() const
	{
		return RTV;
	}

	
	ID3D11DepthStencilView*	GetDSV() const
	{
		return DSV;
	}
	ID3D11ShaderResourceView* GetSRV() const
	{
		return SRV;
	}
	
private:
	D3D11_TEXTURE2D_DESC Desc;

	ID3D11Texture2D* Texture2D = nullptr;

	ID3D11RenderTargetView* RTV = nullptr; // 수정대상으로 삼거나 수정할수 있는 권한.
	ID3D11ShaderResourceView* SRV = nullptr; // 쉐이더에 세팅해줄수 있는 권한
	ID3D11DepthStencilView* DSV = nullptr; // 깊이버퍼를 세팅해줄수 있는 권한
	

	// D3D11_FILTER Filter;
	// D3D11_TEXTURE_ADDRESS_MODE Address;

	void ResLoad(const FString& InPath);
	void ResCreate(const D3D11_TEXTURE2D_DESC& Desc);
	void ResCreate(ID3D11Texture2D* InRes);


	void ResLoad(const TArray<FString>& InPaths);
	HRESULT CreateTexture2DArrayFromFiles(ID3D11Device* device, const std::vector<std::wstring>& fileNames, ID3D11Texture2D** textureArrayOut, ID3D11ShaderResourceView** srvOut);
	HRESULT LoadWICTextureDataFromFile(ID3D11Device* device, const std::wstring& fileName, std::vector<BYTE>& imageData, UINT* width, UINT* height);

};
