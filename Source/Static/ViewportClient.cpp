#include "ViewportClient.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/RenderResourceCollection.h"
#include "Core/Engine.h"
#include "Object/Actor/Camera.h"
#include "Object/World/World.h"

void FViewportClient::Create()
{	
	Viewport = new FViewport(UEngine::Get().GetScreenWidth(), UEngine::Get().GetScreenHeight());

	RenderResourceCollection.SetConstantBufferBinding("FConstantsComponentData", &ConstantsComponentData, 0, true, false);
	RenderResourceCollection.SetMesh("Cube");
	RenderResourceCollection.SetMaterial("DefaultMaterial");
}

void FViewportClient::Render()
{
	ACamera* Camera = UEngine::Get().GetWorld()->GetCamera();

	FMatrix ModelMatrix = FMatrix::Identity();
	ModelMatrix.M[0][0] = 2.0f;
	ModelMatrix.M[1][1] = 0.002f;
	ModelMatrix.M[3][1] = -Viewport->SplitterH->Pos;

	const FMatrix& ProjectionMatrix = FMatrix::OrthoForLH(1, 1, Camera->GetNear(), Camera->GetFar());

	FMatrix MVP = FMatrix::Transpose(
		ModelMatrix *
		ProjectionMatrix
	);

	FConstantsComponentData& Data = GetConstantsComponentData();

	Data.MVP = MVP;
	Data.bUseVertexColor = true;

	// 렌더링 수행
	RenderResourceCollection.Render();

	ModelMatrix.M[0][0] = 0.001f;
	ModelMatrix.M[1][1] = 2.0f;
	ModelMatrix.M[3][0] = Viewport->SplitterV->Pos;

	MVP = FMatrix::Transpose(
		ModelMatrix *
		ProjectionMatrix
	);

	Data.MVP = MVP;
	
	RenderResourceCollection.Render();
}

