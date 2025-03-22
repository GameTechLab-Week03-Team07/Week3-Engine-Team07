#pragma once
#include "Core/AbstractClass/Singleton.h"
#include "Primitive/PrimitiveVertices.h"
#include "Resource/DirectResource/IndexBuffer.h"
#include "Resource/DirectResource/Vertexbuffer.h"
#include "Resource/RenderResourceCollection.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Core/Rendering/Viewport.h"



class FViewportClient : public TSingleton<FViewportClient>
{
public:
	void Create();
	void Render();
	void SetMesh(const FString& InName) { RenderResourceCollection.SetMesh(InName); }
	void SetMaterial(const FString& InName) { RenderResourceCollection.SetMaterial(InName); }
	FConstantsComponentData& GetConstantsComponentData() { return ConstantsComponentData; }

private:
	TArray<FVertexSimple> VertexBuffer;
	TArray<uint32> IndexBuffer;
	FRenderResourceCollection RenderResourceCollection;
	FConstantsComponentData ConstantsComponentData;
	FViewport* Viewport;
};