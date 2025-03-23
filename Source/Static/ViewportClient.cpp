#include "ViewportClient.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/RenderResourceCollection.h"
#include "Core/Engine.h"
#include "Object/Actor/Camera.h"
#include "Object/World/World.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Rendering/FDevice.h"

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
	ModelMatrix.M[1][1] = 0.001f;
	ModelMatrix.M[3][1] = Viewport->SplitterH->Pos / 2;

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
	ModelMatrix.M[3][0] = Viewport->SplitterV->Pos / 2;

	MVP = FMatrix::Transpose(
		ModelMatrix *
		ProjectionMatrix
	);

	Data.MVP = MVP;
	
	RenderResourceCollection.Render();
}

void FViewportClient::Drag() {
	if (APlayerInput::Get().GetKeyDown(EKeyCode::LButton) == true)
	{
		FVector MouseNDCPos = APlayerInput::Get().GetMouseNDCPos();
		// SplitterH

		Viewport->SplitterH->Pos = APlayerInput::Get().GetMouseNDCPos().Y;
		Viewport->SplitterH->SideLT->Rect.Bottom = APlayerInput::Get().GetMouseNDCPos().Y;
		Viewport->SplitterH->SideRB->Rect.Top = APlayerInput::Get().GetMouseNDCPos().Y;

		// SplitterV
		Viewport->SplitterV->Pos = APlayerInput::Get().GetMouseNDCPos().X;
		Viewport->SplitterV->SideLT->Rect.Right = APlayerInput::Get().GetMouseNDCPos().X;
		Viewport->SplitterV->SideRB->Rect.Left = APlayerInput::Get().GetMouseNDCPos().X;

		float WindowWidth = UEngine::Get().GetScreenWidth();
		float WindowHeight = UEngine::Get().GetScreenHeight();

		FDevice::Get().UpdateViewport(Viewport);
	}
}