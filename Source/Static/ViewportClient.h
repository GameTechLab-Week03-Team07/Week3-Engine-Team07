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

	void Drag();
	void UpdateDragState();

	inline void SetMesh(const FString& InName) { RenderResourceCollection.SetMesh(InName); }
	inline void SetMaterial(const FString& InName) { RenderResourceCollection.SetMaterial(InName); }

	inline FConstantsComponentData& GetConstantsComponentData() { return ConstantsComponentData; }

	uint32 GetHoveredViewport();
	void SetFocusedViewport();

	inline uint32 GetFocusedViewportIndex() const { return FocusedViewportIndex; }
	inline void SetFocusedViewportIndex(uint32 index) { FocusedViewportIndex = index; }

	inline std::shared_ptr<FViewport> GetViewport(uint32 index) { return Viewports[index]; }
	inline void AddViewport(int32 InX, int32 InY, int32 InWidth, int32 InHeight) { Viewports.Add(std::make_shared<FViewport>(InX, InY, InWidth, InHeight)); }

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

	uint32 FocusedViewportIndex = 2;
};