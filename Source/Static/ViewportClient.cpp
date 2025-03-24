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
	// 루트	윈도우 생성
	RootWindow = std::make_shared<SWindow>(-1.0f, 1.0f, 1.0f, -1.0f);
	
	// 수평 스플리터 생성
	SplitterH = std::make_shared<SSplitterH>(-1.0f, 0.01f, 1.0f, -0.01f);

	// 수직 스플리터 생성
	SplitterV_Top = std::make_shared<SSplitterV>(-0.01f, 1.0f, 0.01f, 0.0f);
	SplitterV_Bottom = std::make_shared<SSplitterV>(-0.01f, 0.0f, 0.01f, -1.0f);

	// 분할 뷰포트 생성
	AddViewport(-1.0f, 1.0f, 1.0f, 1.0f);
	AddViewport(0.0f, 1.0f, 1.0f, 1.0f);
	AddViewport(-1.0f, 0.0f, 1.0f, 1.0f);
	AddViewport(0.0f, 0.0f, 1.0f, 1.0f);

	// 수직 스플리터에 뷰포트 부착
	SplitterV_Top->SideLT = Viewports[0];
	SplitterV_Top->SideRB = Viewports[1];
	SplitterV_Bottom->SideLT = Viewports[2];
	SplitterV_Bottom->SideRB = Viewports[3];

	// 수평 스플리터에 수직 스플리터 부착
	SplitterH->SideLT = SplitterV_Top;
	SplitterH->SideRB = SplitterV_Bottom;

	// 바인딩
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
	ModelMatrix.M[3][1] = (SplitterH->Rect.Top - SplitterH->Rect.GetHeight() / 2) / 2;

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
	ModelMatrix.M[1][1] = 1.0f;
	ModelMatrix.M[3][0] = (SplitterV_Top->Rect.Left + SplitterV_Top->Rect.GetWidth() / 2) / 2;
	ModelMatrix.M[3][1] = 0.0f;

	MVP = FMatrix::Transpose(
		ModelMatrix *
		ProjectionMatrix
	);

	Data.MVP = MVP;
	
	RenderResourceCollection.Render();

	ModelMatrix.M[0][0] = 0.001f;
	ModelMatrix.M[1][1] = 1.0f;
	ModelMatrix.M[3][0] = (SplitterV_Bottom->Rect.Left + SplitterV_Bottom->Rect.GetWidth() / 2) / 2;
	float temp = SplitterV_Bottom->Rect.GetWidth() / 2;
	ModelMatrix.M[3][1] = 0.0f;

	MVP = FMatrix::Transpose(
		ModelMatrix *
		ProjectionMatrix
	);

	Data.MVP = MVP;

	RenderResourceCollection.Render();
}

void FViewportClient::Drag() 
{
	UpdateDragState();

	if (APlayerInput::Get().GetKeyDown(EKeyCode::LButton) || APlayerInput::Get().GetKeyPress(EKeyCode::LButton))
	{
		FVector MouseNDCPos = APlayerInput::Get().GetMouseNDCPos();
		
		// SplitterH
		if (bIsDragSplitterH)
		{
			SplitterH->Rect.Top = MouseNDCPos.Y + 0.002f;
			SplitterH->Rect.Bottom = MouseNDCPos.Y - 0.002f;

			// SplitterV 위치 설정
			SplitterV_Top->Rect.Top = 1.0f; 
			SplitterV_Bottom->Rect.Top = SplitterH->Rect.Bottom;
			SplitterV_Top->Rect.Bottom = SplitterH->Rect.Top;
			SplitterV_Bottom->Rect.Bottom = -1.0f;

			// 뷰포트 위치 설정
			SplitterV_Top->SideLT->Rect.Bottom = SplitterH->Rect.Top;
			SplitterV_Top->SideRB->Rect.Bottom = SplitterH->Rect.Top;
			SplitterV_Bottom->SideLT->Rect.Top = SplitterH->Rect.Bottom;
			SplitterV_Bottom->SideRB->Rect.Top = SplitterH->Rect.Bottom;

			Viewports[0]->SetY(SplitterV_Top->SideLT->Rect.Top);
			Viewports[1]->SetY(SplitterV_Top->SideRB->Rect.Top);
			Viewports[2]->SetY(SplitterV_Bottom->SideLT->Rect.Top);
			Viewports[3]->SetY(SplitterV_Bottom->SideRB->Rect.Top);

			Viewports[0]->SetHeight(SplitterV_Top->SideLT->Rect.GetHeight());
			Viewports[1]->SetHeight(SplitterV_Top->SideRB->Rect.GetHeight());
			Viewports[2]->SetHeight(SplitterV_Bottom->SideLT->Rect.GetHeight());
			Viewports[3]->SetHeight(SplitterV_Bottom->SideRB->Rect.GetHeight());
		}
		
		// SplitterV
		else if (bIsDragSplitterV)
		{
			SplitterV_Top->Rect.Left = MouseNDCPos.X - 0.002f;
			SplitterV_Bottom->Rect.Left = MouseNDCPos.X - 0.002f;
			
			SplitterV_Top->Rect.Right = MouseNDCPos.X + 0.002f;
			SplitterV_Bottom->Rect.Right = MouseNDCPos.X + 0.002f;

			// 뷰포트 위치 설정
			SplitterV_Top->SideLT->Rect.Right = SplitterV_Top->Rect.Left;
			SplitterV_Top->SideRB->Rect.Left = SplitterV_Top->Rect.Right;
			SplitterV_Bottom->SideLT->Rect.Right = SplitterV_Bottom->Rect.Left;
			SplitterV_Bottom->SideRB->Rect.Left = SplitterV_Bottom->Rect.Right;

			Viewports[0]->SetX(SplitterV_Top->SideLT->Rect.Left);
			Viewports[1]->SetX(SplitterV_Top->SideRB->Rect.Left);
			Viewports[2]->SetX(SplitterV_Bottom->SideLT->Rect.Left);
			Viewports[3]->SetX(SplitterV_Bottom->SideRB->Rect.Left);

			Viewports[0]->SetWidth(SplitterV_Top->SideLT->Rect.GetWidth());
			Viewports[1]->SetWidth(SplitterV_Top->SideRB->Rect.GetWidth());
			Viewports[2]->SetWidth(SplitterV_Bottom->SideLT->Rect.GetWidth());
			Viewports[3]->SetWidth(SplitterV_Bottom->SideRB->Rect.GetWidth());
		}

		FDevice::Get().UpdateViewport();

		Render();
	}
	
	if(APlayerInput::Get().GetKeyUp(EKeyCode::LButton))
	{
		bIsDragSplitterH = false;
		bIsDragSplitterV = false;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

void FViewportClient::UpdateDragState() 
{
	if (SplitterH->IsHover(APlayerInput::Get().GetMouseNDCPos()))
	{
		SetCursor(LoadCursor(NULL, IDC_SIZENS));
		if (APlayerInput::Get().GetKeyPress(EKeyCode::LButton))
		{
			bIsDragSplitterH = true;
		}
	}
	else if (SplitterV_Top->IsHover(APlayerInput::Get().GetMouseNDCPos()))
	{
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		if (APlayerInput::Get().GetKeyPress(EKeyCode::LButton))
		{
			bIsDragSplitterV = true;
		}
	}
	else if (SplitterV_Bottom->IsHover(APlayerInput::Get().GetMouseNDCPos()))
	{
		SetCursor(LoadCursor(NULL, IDC_SIZEWE));
		if (APlayerInput::Get().GetKeyPress(EKeyCode::LButton))
		{
			bIsDragSplitterV = true;
		}
	}
}

uint32 FViewportClient::GetHoveredViewport() {
	for (int i = 0; i < 4; i++)
	{
		if (FViewportClient::Get().GetViewport(i)->IsHover(APlayerInput::Get().GetMouseNDCPos()))
		{
			return i;
		}
	}
}

void FViewportClient::SetFocusedViewport() {
	for (int i = 0; i < 4; i++)
	{
		if (FViewportClient::Get().GetViewport(i)->IsHover(APlayerInput::Get().GetMouseNDCPos()) && APlayerInput::Get().GetKeyPress(EKeyCode::LButton))
		{
			SetFocusedViewportIndex(i);
		}
	}
}