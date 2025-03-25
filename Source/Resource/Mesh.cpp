#include "Mesh.h"


void FMesh::Setting()
{
	if (nullptr == VertexBuffer)
	{
		MsgBoxAssert("매쉬가 세팅되어 있지 않습니다.");
		return;
	}
	VertexBuffer->Setting();

	FDevice::Get().GetDeviceContext()->IASetPrimitiveTopology(Topology);

	if (nullptr == IndexBuffer)
	{
		MsgBoxAssert("매쉬가 세팅되어 있지 않습니다.");
		return;
	}
	IndexBuffer->Setting();
}

void FMesh::Draw()
{
	
	FDevice::Get().GetDeviceContext()->DrawIndexed(IndexBuffer->GetIndexCount(), 0, 0);
}

void FMesh::Draw(const TArray<FSubMeshSection>& Sections)
{
	// 각 서브메시 섹션에 대해 별도의 드로우 콜을 수행
	for (const auto& Section : Sections)
	{
		FDevice::Get().GetDeviceContext()->DrawIndexed(Section.indexCount, Section.indexStart, 0);
	}
}

