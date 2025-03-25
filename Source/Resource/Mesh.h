#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include "Core/Engine.h"
#include "Core/Rendering/FDevice.h"
#include "StaticMesh/StaticMeshTypes.h"
#include "Resource/Resource.h"
#include "Core/Container/String.h"
#include "Core/Container/Array.h"
#include "DirectResource/Shader.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Debug/DebugConsole.h"

class FMesh : public FResource<FMesh>
{
public:
	FMesh() = default;

	static std::shared_ptr<FMesh> Create(const FString& InName, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		return Create(InName, InName, InName, Topology);
	}

	static std::shared_ptr<FMesh> Create(
		const FString& InName, const FString& VertexName, const FString& IndexName
		, D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	)
	{
		std::shared_ptr<FMesh> Res = CreateRes(InName);
		Res->VertexBuffer = FVertexBuffer::Find(VertexName);
		Res->IndexBuffer = FIndexBuffer::Find(IndexName);
		Res->Topology = Topology;

		if (nullptr == Res->VertexBuffer
			|| nullptr == Res->IndexBuffer)
		{
			MsgBoxAssert("매쉬를 만드는데 실패했습니다.");
		}

		return Res;
	}

	static std::shared_ptr<FMesh> Create(
		const FString& InName, const std::shared_ptr<FVertexBuffer>& InVertex, const std::shared_ptr<FIndexBuffer>& InIndex,
		D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST
	)
	{
		std::shared_ptr<FMesh> Res = CreateRes(InName);
		Res->VertexBuffer = InVertex;
		Res->IndexBuffer = InIndex;
		Res->Topology = Topology;

		if (nullptr == Res->VertexBuffer
			|| nullptr == Res->IndexBuffer)
		{
			MsgBoxAssert("매쉬를 만드는데 실패했습니다.");
		}

		return Res;
	}

	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY InTopology)
	{
		Topology = InTopology;
	}

	void Setting();
	void Draw();

	void Draw(const TArray<FSubMeshSection>& Sections);

	void SetSections(const TArray<FSubMeshSection>& InSections) { Sections = InSections; }
	const TArray<FSubMeshSection>& GetSections() const { return Sections; }

	std::shared_ptr<FVertexBuffer> GetVertexBuffer()
	{
		return VertexBuffer;
	}

	std::shared_ptr<FIndexBuffer> GetIndexBuffer()
	{
		return IndexBuffer;
	}
	
private:
	std::shared_ptr<FVertexBuffer> VertexBuffer = nullptr;
	std::shared_ptr<FIndexBuffer> IndexBuffer = nullptr;
	D3D_PRIMITIVE_TOPOLOGY Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	TArray<FSubMeshSection> Sections;
};
