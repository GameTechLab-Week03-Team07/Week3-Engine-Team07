#include "RenderResourceCollection.h"
#include "Debug/DebugConsole.h"
#include "DirectResource/ShaderResourceBinding.h"
#include "DirectResource/InputLayout.h"
#include "Mesh.h"
#include "Material.h"

#include "Core/Rendering/FDevice.h"

void FRenderResourceCollection::SetMesh(const FString& _Name)
{
	Mesh = FMesh::Find(_Name);

	SetMesh(Mesh);
}

void FRenderResourceCollection::SetMesh(std::shared_ptr<FMesh> _Mesh)
{
	Mesh = _Mesh;

	if (Mesh == nullptr) MsgBoxAssert("존재하지 않는 매쉬를 세팅하려고 했습니다.");

	//if (nullptr == Layout && nullptr != Material)
	if (Layout == nullptr && Material != nullptr)
	{
		Layout = std::make_shared<FInputLayout>();
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::SetMaterial(const FString& _Name)
{
	Material = FMaterial::Find(_Name);

	SetMaterial(Material);
}

void FRenderResourceCollection::SetMaterial(std::shared_ptr<FMaterial> _Material)
{
	Material = _Material;


	if (Material == nullptr) MsgBoxAssert("존재하지 않는 머티리얼을 세팅하려고 했습니다.");

	if (Layout == nullptr && Material != nullptr)
	{
		Layout = std::make_shared<FInputLayout>();

		// ResCreateForTextuerArray함수를 추가해야 함.
		Layout->ResCreate(Material->GetVertexShader());
	}
}

void FRenderResourceCollection::Render()
{
	Mesh->Setting();
	Layout->Setting();
	Material->Setting(); 

	for (auto& Binding : ConstantBufferBindings)
	{
		Binding.Value->Setting();
	}
  
  for (auto& Binding : TextureBindings)
	{
		Binding.Value->Setting();
	}
	
	for (auto& Binding : SamplerBindings)
	{
		Binding.Value->Setting();
	}
	
	Mesh->Draw();
}

void FRenderResourceCollection::Render(const FSubMeshSection& section)
{
	Mesh->Setting();
	Layout->Setting();
	Material->Setting();

	for (auto& Binding : ConstantBufferBindings)
	{
		Binding.Value->Setting();
	}

	for (auto& Binding : TextureBindings)
	{
		Binding.Value->Setting();
	}

	for (auto& Binding : SamplerBindings)
	{
		Binding.Value->Setting();
	}
	Mesh->Draw(section);
}


void FRenderResourceCollection::Reset()
{
	for (auto& Binding : TextureBindings)
	{
		Binding.Value->Reset();
	}
}

std::shared_ptr<FTextureBinding> FRenderResourceCollection::SetTextureBinding(const FString& _Name, int _BindPoint,
                                                                              bool bIsUseVertexShader, bool bIsUsePixelShader)
{
	std::shared_ptr<FTexture> Res = FTexture::Find(_Name);

	// if (Res == nullptr)
	// {
	// 	//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
	// 	Res = FConstantBuffer::Create(_Name, _DataSize);
	// }
	
	std::shared_ptr<FTextureBinding> Binding = std::make_shared<FTextureBinding>();

	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	TextureBindings.Add(_Name, Binding);

	return Binding;
}

std::shared_ptr<FSamplerBinding> FRenderResourceCollection::SetSamplerBinding(const FString& _Name, int _BindPoint,
                                                                              bool bIsUseVertexShader, bool bIsUsePixelShader)
{
	std::shared_ptr< FSampler> Res = FSampler::Find(_Name);

	// if (Res == nullptr)
	// {
	// 	//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
	// 	Res = FConstantBuffer::Create(_Name, _DataSize);
	// }
	
	std::shared_ptr<FSamplerBinding> Binding = std::make_shared<FSamplerBinding>();

	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	SamplerBindings.Add(_Name, Binding);

	return Binding;
}

std::shared_ptr<FConstantBufferBinding> FRenderResourceCollection::SetConstantBufferBinding(const FString& _Name,
                                                                                            const void* _CPUDataPtr, int _DataSize, int _BindPoint, bool bIsUseVertexShader, bool bIsUsePixelShader)
{

	/*std::shared_ptr<class FConstantBufferBinding>* Binding = ConstantBufferBindings.Find(_Name);

	if()*/
	std::shared_ptr<FConstantBuffer> Res = FConstantBuffer::Find(_Name);

	if (Res == nullptr)
	{
		//없으면 그 사이즈의 이름으로 만듦 이름이 겹치면 몰?루
		Res = FConstantBuffer::Create(_Name, _DataSize);
	}
	
	std::shared_ptr<FConstantBufferBinding> Binding = std::make_shared<FConstantBufferBinding>();

	Binding->DataSize = _DataSize;
	Binding->CPUDataPtr = _CPUDataPtr;
	Binding->Res = Res;
	Binding->Name = _Name;
	Binding->bIsUseVertexShader = bIsUseVertexShader;
	Binding->bIsUsePixelShader = bIsUsePixelShader;
	Binding->BindPoint = _BindPoint;

	ConstantBufferBindings.Add(_Name, Binding);

	return Binding;
}

// Helper: 현재 예제에서는 R8G8B8A8_UNORM 포맷만 처리 (필요 시 다른 포맷 추가)
static int GetBytesPerPixel(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return 4;
	default:
		return 4;
	}
}

std::shared_ptr<FShaderResourceBinding> FRenderResourceCollection::SetTextureArrayBinding(
	const FString& _Name,
	int _BindPoint,
	bool bIsUseVertexShader,
	bool bIsUsePixelShader,
	const std::vector<void*>& textureData,  // 각 머티리얼의 diffuse 텍스처의 base mip level 데이터 포인터 배열
	int numTextures,
	int width,
	int height,
	int mipLevels,
	DXGI_FORMAT textureFormat)
{
	// 1. Texture2DArray 생성을 위한 디스크립터 설정
	D3D11_TEXTURE2D_DESC textureArrayDesc = {};
	textureArrayDesc.Width = width;
	textureArrayDesc.Height = height;
	textureArrayDesc.MipLevels = mipLevels;
	textureArrayDesc.ArraySize = numTextures;
	textureArrayDesc.Format = textureFormat;
	textureArrayDesc.SampleDesc.Count = 1;
	textureArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	textureArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	// 2. 각 텍스처의 base level 데이터로 서브리소스 데이터 배열 구성
	// (여기서는 mipLevels가 1이라고 가정)
	std::vector<D3D11_SUBRESOURCE_DATA> subresources(numTextures);
	int bytesPerPixel = GetBytesPerPixel(textureFormat);
	for (int i = 0; i < numTextures; ++i)
	{
		D3D11_SUBRESOURCE_DATA& subData = subresources[i];
		subData.pSysMem = textureData[i];
		subData.SysMemPitch = width * bytesPerPixel;
		subData.SysMemSlicePitch = width * height * bytesPerPixel;
	}

	// 3. 디바이스 포인터 획득
	ID3D11Device* device = FDevice::Get().GetDevice();
	if (!device)
	{
		// device 오류 처리
		return nullptr;
	}

	// 4. Texture2DArray 생성
	ID3D11Texture2D* textureArray = nullptr;
	HRESULT hr = device->CreateTexture2D(&textureArrayDesc, subresources.data(), &textureArray);
	if (FAILED(hr))
	{
		// 생성 실패 처리 (로그 등)
		return nullptr;
	}

	// 5. Shader Resource View (SRV) 생성
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureArrayDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = textureArrayDesc.MipLevels;
	srvDesc.Texture2DArray.ArraySize = textureArrayDesc.ArraySize;
	srvDesc.Texture2DArray.FirstArraySlice = 0;

	ID3D11ShaderResourceView* textureArraySRV = nullptr;
	hr = device->CreateShaderResourceView(textureArray, &srvDesc, &textureArraySRV);
	if (FAILED(hr))
	{
		textureArray->Release();
		return nullptr;
	}

	// textureArray를 직접 참조하지 않아도 되도록 SRV가 내부에서 참조하므로 해제
	textureArray->Release();

	// 6. 새 바인딩 객체(FTextureArrayBindingEx) 생성 및 설정
	std::shared_ptr<FTextureArrayBinding> binding = std::make_shared<FTextureArrayBinding>();
	binding->Name = _Name;
	binding->bIsUseVertexShader = bIsUseVertexShader;
	binding->bIsUsePixelShader = bIsUsePixelShader;
	binding->BindPoint = _BindPoint;
	binding->SRV = textureArraySRV;

	// 필요 시, 내부적으로 관리하기 위해 TextureBindings 맵에 추가할 수 있습니다.
	// TextureBindings.Add(_Name, binding);

	return binding;
}
