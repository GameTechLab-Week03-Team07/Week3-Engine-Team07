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
	void Drag();
	void UpdateDragState();
	std::shared_ptr<FViewport> GetViewport(uint32 index) { return Viewports[index]; }
	void AddViewport(int32 InX, int32 InY, int32 InWidth, int32 InHeight) { Viewports.Add(std::make_shared<FViewport>(InX, InY, InWidth, InHeight)); }

private:
	TArray<FVertexSimple> VertexBuffer;
	TArray<uint32> IndexBuffer;
	FRenderResourceCollection RenderResourceCollection;
	FConstantsComponentData ConstantsComponentData;
	
	TArray<std::shared_ptr<FViewport>> Viewports;
	std::shared_ptr<SWindow> RootWindow = nullptr;
	std::shared_ptr<SSplitterH> SplitterH = nullptr;
	std::shared_ptr<SSplitterV> SplitterV_Top = nullptr;
	std::shared_ptr<SSplitterV> SplitterV_Bottom = nullptr;
	bool bIsDragSplitterH = false;
	bool bIsDragSplitterV = false;
};