#include "ViewportClient.h"
#include "Resource/Mesh.h"
#include "Resource/Material.h"
#include "Resource/RenderResourceCollection.h"
#include "Core/Engine.h"
#include "Object/Actor/Camera.h"
#include "Object/World/World.h"
#include "Core/Input/PlayerInput.h"
#include "Core/Rendering/FDevice.h"
#include "Core/Rendering/FViewMode.h"
#include "Core/Config/ConfigManager.h"

void FViewportClient::Create()
{	
	// 루트	윈도우 생성
	RootWindow = std::make_shared<SWindow>(-1.0f, 1.0f, 1.0f, -1.0f);
	
	// 수평 스플리터 생성
	float SplitterH_Left = FString::ToFloat(UConfigManager::Get().GetValue("SplitterH", "Left"));
	float SplitterH_Top = FString::ToFloat(UConfigManager::Get().GetValue("SplitterH", "Top"));
	float SplitterH_Right = FString::ToFloat(UConfigManager::Get().GetValue("SplitterH", "Right"));
	float SplitterH_Bottom = FString::ToFloat(UConfigManager::Get().GetValue("SplitterH", "Bottom"));

	SplitterH = std::make_shared<SSplitterH>(SplitterH_Left, SplitterH_Top, SplitterH_Right, SplitterH_Bottom);

	// 수직 스플리터 생성
	float SplitterV_Top_Left = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Top", "Left"));
	float SplitterV_Top_Top = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Top", "Top"));
	float SplitterV_Top_Right = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Top", "Right"));
	float SplitterV_Top_Bottom = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Top", "Bottom"));

	SplitterV_Top = std::make_shared<SSplitterV>(SplitterV_Top_Left, SplitterV_Top_Top, SplitterV_Top_Right, SplitterV_Top_Bottom);

	float SplitterV_Bottom_Left = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Bottom", "Left"));
	float SplitterV_Bottom_Top = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Bottom", "Top"));
	float SplitterV_Bottom_Right = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Bottom", "Right"));
	float SplitterV_Bottom_Bottom = FString::ToFloat(UConfigManager::Get().GetValue("SplitterV_Bottom", "Bottom"));

	SplitterV_Bottom = std::make_shared<SSplitterV>(SplitterV_Bottom_Left, SplitterV_Bottom_Top, SplitterV_Bottom_Right, SplitterV_Bottom_Bottom);

	// 분할 뷰포트 생성
	float ViewportLT_X = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLT", "X"));
	float ViewportLT_Y = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLT", "Y"));
	float ViewportLT_Width = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLT", "Width"));
	float ViewportLT_Height = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLT", "Height"));

	float ViewportRT_X = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRT", "X"));
	float ViewportRT_Y = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRT", "Y"));
	float ViewportRT_Width = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRT", "Width"));
	float ViewportRT_Height = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRT", "Height"));

	float ViewportLB_X = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLB", "X"));
	float ViewportLB_Y = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLB", "Y"));
	float ViewportLB_Width = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLB", "Width"));
	float ViewportLB_Height = FString::ToFloat(UConfigManager::Get().GetValue("ViewportLB", "Height"));

	float ViewportRB_X = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRB", "X"));
	float ViewportRB_Y = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRB", "Y"));
	float ViewportRB_Width = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRB", "Width"));
	float ViewportRB_Height = FString::ToFloat(UConfigManager::Get().GetValue("ViewportRB", "Height"));

	AddViewport(ViewportLT_X, ViewportLT_Y, ViewportLT_Width, ViewportLT_Height);
	AddViewport(ViewportRT_X, ViewportRT_Y, ViewportRT_Width, ViewportRT_Height);
	AddViewport(ViewportLB_X, ViewportLB_Y, ViewportLB_Width, ViewportLB_Height);
	AddViewport(ViewportRB_X, ViewportRB_Y, ViewportRB_Width, ViewportRB_Height);

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
	RenderResourceCollection.SetMaterial("AlwaysVisibleMaterial");
	RenderResourceCollection.SetIsOverrideRasterizer(false);
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
	IsPressedOnSplitter();
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
		bIsPressedOnSplitter = false;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}

void FViewportClient::UpdateDragState() 
{
	// 처음 클릭한 위치가 스플리터인지 확인
	if (APlayerInput::Get().GetKeyPress(EKeyCode::LButton) && !bIsPressedOnSplitter) 
		return;

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

void FViewportClient::SetFocusedViewport() 
{
	for (int i = 0; i < 4; i++)
	{
		if (FViewportClient::Get().GetViewport(i)->IsHover(APlayerInput::Get().GetMouseNDCPos()))
		{
			SetFocusedViewportIndex(i);
			UEngine::Get().GetWorld()->GetCameraList()[OrthoViewModeLookup[i]]->SetIsControllable(true);
			continue;
		}
		UEngine::Get().GetWorld()->GetCameraList()[OrthoViewModeLookup[i]]->SetIsControllable(false);
	}
}

void FViewportClient::IsPressedOnSplitter() 
{
	if (APlayerInput::Get().GetKeyDown(EKeyCode::LButton))
	{
		if (SplitterH->IsHover(APlayerInput::Get().GetMouseNDCPos()) || 
			SplitterV_Top->IsHover(APlayerInput::Get().GetMouseNDCPos()) || 
			SplitterV_Bottom->IsHover(APlayerInput::Get().GetMouseNDCPos()))
		{
			bIsPressedOnSplitter = true;
		}
	}
}

void FViewportClient::SaveViewport() 
{
	UConfigManager::Get().SetValue("SplitterH", "Left", std::to_string(FViewportClient::Get().GetSplitterH()->Rect.Left));
	UConfigManager::Get().SetValue("SplitterH", "Top", std::to_string(FViewportClient::Get().GetSplitterH()->Rect.Top));
	UConfigManager::Get().SetValue("SplitterH", "Right", std::to_string(FViewportClient::Get().GetSplitterH()->Rect.Right));
	UConfigManager::Get().SetValue("SplitterH", "Bottom", std::to_string(FViewportClient::Get().GetSplitterH()->Rect.Bottom));

	UConfigManager::Get().SetValue("SplitterV_Top", "Left", std::to_string(FViewportClient::Get().GetSplitterV_Top()->Rect.Left));
	UConfigManager::Get().SetValue("SplitterV_Top", "Top", std::to_string(FViewportClient::Get().GetSplitterV_Top()->Rect.Top));
	UConfigManager::Get().SetValue("SplitterV_Top", "Right", std::to_string(FViewportClient::Get().GetSplitterV_Top()->Rect.Right));
	UConfigManager::Get().SetValue("SplitterV_Top", "Bottom", std::to_string(FViewportClient::Get().GetSplitterV_Top()->Rect.Bottom));

	UConfigManager::Get().SetValue("SplitterV_Bottom", "Left", std::to_string(FViewportClient::Get().GetSplitterV_Bottom()->Rect.Left));
	UConfigManager::Get().SetValue("SplitterV_Bottom", "Top", std::to_string(FViewportClient::Get().GetSplitterV_Bottom()->Rect.Top));
	UConfigManager::Get().SetValue("SplitterV_Bottom", "Right", std::to_string(FViewportClient::Get().GetSplitterV_Bottom()->Rect.Right));
	UConfigManager::Get().SetValue("SplitterV_Bottom", "Bottom", std::to_string(FViewportClient::Get().GetSplitterV_Bottom()->Rect.Bottom));

	UConfigManager::Get().SetValue("ViewportLT", "X", std::to_string(FViewportClient::Get().GetViewport(0)->GetX()));
	UConfigManager::Get().SetValue("ViewportLT", "Y", std::to_string(FViewportClient::Get().GetViewport(0)->GetY()));
	UConfigManager::Get().SetValue("ViewportLT", "Width", std::to_string(FViewportClient::Get().GetViewport(0)->GetWidth()));
	UConfigManager::Get().SetValue("ViewportLT", "Height", std::to_string(FViewportClient::Get().GetViewport(0)->GetHeight()));

	UConfigManager::Get().SetValue("ViewportRT", "X", std::to_string(FViewportClient::Get().GetViewport(1)->GetX()));
	UConfigManager::Get().SetValue("ViewportRT", "Y", std::to_string(FViewportClient::Get().GetViewport(1)->GetY()));
	UConfigManager::Get().SetValue("ViewportRT", "Width", std::to_string(FViewportClient::Get().GetViewport(1)->GetWidth()));
	UConfigManager::Get().SetValue("ViewportRT", "Height", std::to_string(FViewportClient::Get().GetViewport(1)->GetHeight()));

	UConfigManager::Get().SetValue("ViewportLB", "X", std::to_string(FViewportClient::Get().GetViewport(2)->GetX()));
	UConfigManager::Get().SetValue("ViewportLB", "Y", std::to_string(FViewportClient::Get().GetViewport(2)->GetY()));
	UConfigManager::Get().SetValue("ViewportLB", "Width", std::to_string(FViewportClient::Get().GetViewport(2)->GetWidth()));
	UConfigManager::Get().SetValue("ViewportLB", "Height", std::to_string(FViewportClient::Get().GetViewport(2)->GetHeight()));

	UConfigManager::Get().SetValue("ViewportRB", "X", std::to_string(FViewportClient::Get().GetViewport(3)->GetX()));
	UConfigManager::Get().SetValue("ViewportRB", "Y", std::to_string(FViewportClient::Get().GetViewport(3)->GetY()));
	UConfigManager::Get().SetValue("ViewportRB", "Width", std::to_string(FViewportClient::Get().GetViewport(3)->GetWidth()));
	UConfigManager::Get().SetValue("ViewportRB", "Height", std::to_string(FViewportClient::Get().GetViewport(3)->GetHeight()));
}