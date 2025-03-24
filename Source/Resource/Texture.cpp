#include "Texture.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"

#include "Debug/DebugConsole.h"
#include "DirectXTK/DDSTextureLoader.h"
#include "DirectXTK/WICTextureLoader.h"

#include <wincodec.h>

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


void FTexture::ResLoad(const TArray<FString>& InPaths)
{
	// 텍스처 파일들이 여러 개 전달된 경우, 이들을 하나의 Texture2DArray로 로드합니다.
	// (모든 텍스처는 같은 해상도와 포맷이어야 합니다.)

	// 1. 각 FString을 std::wstring으로 변환
	std::vector<std::wstring> wPaths;
	for (const FString& path : InPaths)
	{
		// Unreal Engine의 FString을 const wchar_t* 로 변환할 수 있는 경우도 있지만,
		// 여기서는 std::wstring으로 명시적으로 변환합니다.
		wPaths.push_back(path.ToWideString());
	}

	// 2. Texture2DArray를 생성하는 함수 호출 (구현은 별도로 필요)
	// 예를 들어, CreateTexture2DArrayFromFiles()는 여러 텍스처 파일을 받아 Texture2DArray와 SRV를 생성합니다.
	ID3D11Texture2D* pTextureArray = nullptr;
	HRESULT hr = CreateTexture2DArrayFromFiles(FDevice::Get().GetDevice(), wPaths, &pTextureArray, &SRV);
	if (FAILED(hr))
	{
		MsgBoxAssert("Texture Array load failed.");
		return;
	}

	// 3. (선택 사항) pTextureArray를 내부 멤버 변수에 저장하거나 추가 초기화 작업 수행
	Texture2D = pTextureArray;
}

HRESULT FTexture::CreateTexture2DArrayFromFiles(ID3D11Device* device, const std::vector<std::wstring>& fileNames, ID3D11Texture2D** textureArrayOut, ID3D11ShaderResourceView** srvOut)
{
	UINT width = 0, height = 0;
	std::vector<std::vector<BYTE>> imageDatas;

	// 모든 파일을 로드
	for (const auto& file : fileNames)
	{
		std::vector<BYTE> imageData;
		UINT w = 0, h = 0;
		HRESULT hr = LoadWICTextureDataFromFile(device, file, imageData, &w, &h);
		if (FAILED(hr))
			return hr;
		if (width == 0 && height == 0)
		{
			width = w;
			height = h;
		}
		else if (w != width || h != height)
		{
			// 모든 텍스처의 크기가 동일해야 Texture2DArray로 생성할 수 있습니다.
			return E_FAIL;
		}
		imageDatas.push_back(std::move(imageData));
	}

	UINT textureCount = static_cast<UINT>(fileNames.size());

	// Texture2DArray 생성에 사용할 설명 구조체 설정
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = textureCount;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	// 각 텍스처의 서브리소스 데이터 설정
	std::vector<D3D11_SUBRESOURCE_DATA> subresources(textureCount);
	UINT rowPitch = width * 4; // 4바이트 per 픽셀
	for (UINT i = 0; i < textureCount; ++i)
	{
		subresources[i].pSysMem = imageDatas[i].data();
		subresources[i].SysMemPitch = rowPitch;
		subresources[i].SysMemSlicePitch = 0;
	}

	// Texture2DArray 생성
	HRESULT hr = device->CreateTexture2D(&texDesc, subresources.data(), textureArrayOut);
	if (FAILED(hr))
		return hr;

	// Shader Resource View (SRV) 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = textureCount;

	hr = device->CreateShaderResourceView(*textureArrayOut, &srvDesc, srvOut);
	return hr;
}

HRESULT FTexture::LoadWICTextureDataFromFile(ID3D11Device* device, const std::wstring& fileName, std::vector<BYTE>& imageData, UINT* width, UINT* height)
{
	// WIC 팩토리 생성
	IWICImagingFactory* pWICFactory = nullptr;
	HRESULT hr = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pWICFactory));
	if (FAILED(hr))
		return hr;

	// 파일로부터 디코더 생성
	IWICBitmapDecoder* pDecoder = nullptr;
	hr = pWICFactory->CreateDecoderFromFilename(
		fileName.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&pDecoder);
	if (FAILED(hr))
	{
		pWICFactory->Release();
		return hr;
	}

	// 첫 번째 프레임 가져오기
	IWICBitmapFrameDecode* pFrame = nullptr;
	hr = pDecoder->GetFrame(0, &pFrame);
	if (FAILED(hr))
	{
		pDecoder->Release();
		pWICFactory->Release();
		return hr;
	}

	// 이미지 크기 얻기
	hr = pFrame->GetSize(width, height);
	if (FAILED(hr))
	{
		pFrame->Release();
		pDecoder->Release();
		pWICFactory->Release();
		return hr;
	}

	// 32비트 RGBA 포맷으로 변환
	IWICFormatConverter* pConverter = nullptr;
	hr = pWICFactory->CreateFormatConverter(&pConverter);
	if (FAILED(hr))
	{
		pFrame->Release();
		pDecoder->Release();
		pWICFactory->Release();
		return hr;
	}

	hr = pConverter->Initialize(
		pFrame,
		GUID_WICPixelFormat32bppRGBA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.f,
		WICBitmapPaletteTypeCustom);
	if (FAILED(hr))
	{
		pConverter->Release();
		pFrame->Release();
		pDecoder->Release();
		pWICFactory->Release();
		return hr;
	}

	// 이미지 데이터를 저장할 크기 계산
	UINT rowPitch = (*width) * 4; // 4바이트 per 픽셀 (RGBA)
	UINT imageSize = rowPitch * (*height);
	imageData.resize(imageSize);

	hr = pConverter->CopyPixels(nullptr, rowPitch, imageSize, imageData.data());

	// 리소스 해제
	pConverter->Release();
	pFrame->Release();
	pDecoder->Release();
	pWICFactory->Release();

	return hr;
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

