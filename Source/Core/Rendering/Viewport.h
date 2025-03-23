#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Math/Matrix.h"
#include "Object/Window/Window.h"


class FViewport
{
public:
	FViewport(int32 InWidth, int32 InHeight)
		: Width(InWidth), Height(InHeight) {

		// 수직, 수평 분할기 생성
		SplitterH = std::make_shared<SSplitterH>(-1.0f, -0.001f, 1.0f, 0.001f);
		SplitterV = std::make_shared<SSplitterV>(-0.001f, -1.0f, 0.001f, 1.0f);

		// 4분할된 뷰포트 생성 및 설정
		SplitterH->SideLT = std::make_shared<SWindow>(-1.0f, 1.0f, 1.0f, 0.0f);
		SplitterH->SideRB = std::make_shared<SWindow>(-1.0f, 0.0f, 1.0f, -1.0f);

		SplitterV->SideLT = std::make_shared<SWindow>(-1.0f, 1.0f, 0.0f, -1.0f);
		SplitterV->SideRB = std::make_shared<SWindow>(0.0f, 1.0f, 1.0f, -1.0f);
	}

	int32 GetX() const { return X; }
	int32 GetY() const { return Y; }
	int32 GetWidth() const { return Width; }
	int32 GetHeight() const { return Height; }
	void SetWidth(int32 InWidth) { Width = InWidth; }
	void SetHeight(int32 InHeight) { Height = InHeight; }

	std::shared_ptr<SSplitterH> SplitterH;
	std::shared_ptr<SSplitterV> SplitterV;

private:
	int32 X, Y, Width, Height;
}; 