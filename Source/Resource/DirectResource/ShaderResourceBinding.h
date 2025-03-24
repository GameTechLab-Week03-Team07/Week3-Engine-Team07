#pragma once
#include "Core/Container/String.h"
#include "ConstantBuffer.h"
#include "Shader.h"
#include "Resource/Texture.h"
#include "Sampler.h"

//리소스와 다르게 각 객체가 가지게 됨
class FShaderResourceBinding
{
public:
	FString Name; // Transformdata <= 쉐이더에서 사용하는 리소스의 이름
	///FShader* ParentShader; // <= 날 사용하는 쉐이더
	bool bIsUseVertexShader = false;
	bool bIsUsePixelShader = false; //어느 쉐이더에서 사용되는 지 
	
	int BindPoint = -1; // <= b0 t0 s0 ... s1 s2 s3, b1 b2 b3 몇번 상수버퍼냐

public:
	virtual void Setting() {}
	virtual void Reset() {}
	
};

//쉐이더가 포인터를 받아 상수버퍼를 바인딩하기 위한 클래스
class FConstantBufferBinding  : public FShaderResourceBinding
{
public:
	friend class FRenderResourceCollection;
	
	std::shared_ptr<FConstantBuffer> Res;
	

	const void* CPUDataPtr = nullptr;
	int DataSize = -1;

	void Setting() override;
	void Reset() override;
};

//쉐이더가 포인터를 받아 텍스쳐를 바인딩하기 위한 클래스
class FTextureBinding  : public FShaderResourceBinding
{
public:
	friend class FRenderResourceCollection;
	
	std::shared_ptr<FTexture> Res;

	void Setting() override;
	void Reset() override;
};

class FTextureArrayBinding : public FShaderResourceBinding {
public:
	friend class FRenderResourceCollection;

	ID3D11ShaderResourceView* SRV = nullptr;

	std::shared_ptr<FTexture> Res;
	void Setting();
	void Reset();
};

//쉐이더가 샘플러를 받아 텍스쳐를 바인딩하기 위한 클래스
class FSamplerBinding  : public FShaderResourceBinding
{
public:
	friend class FRenderResourceCollection;
	
	std::shared_ptr<FSampler> Res;

	void Setting() override;
	void Reset() override;
};