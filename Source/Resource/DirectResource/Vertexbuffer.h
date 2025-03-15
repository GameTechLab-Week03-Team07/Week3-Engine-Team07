#pragma once
#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "DirectBuffer.h"
#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"


class FVertexBuffer :
	public FResource<FVertexBuffer> , public FDirectBuffer
{


public:

	
	template<typename VertexType>
	static std::shared_ptr<FVertexBuffer> Create(const FString&  _Name, const TArray<VertexType>& _Data)
	{
		std::shared_ptr<FVertexBuffer> Res = FVertexBuffer::CreateRes(_Name);
		Res->ResCreate(&_Data[0], sizeof(VertexType), _Data.Num());
		return Res;
	}

	void Setting() const;
	
private:
	
	void ResCreate(const void* _Data, size_t _VertexSize, size_t _VertexCount);


	UINT VertexSize = 0;
	UINT VertexCount = 0;
	UINT Offset = 0;

	
	
};
