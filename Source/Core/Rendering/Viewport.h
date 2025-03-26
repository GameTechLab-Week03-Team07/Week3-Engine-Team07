#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Math/Matrix.h"
#include "Object/Window/Window.h"


class FViewport : public SWindow 
{
public:
	FViewport(float InX, float InY, float InWidth, float InHeight)
		: X(InX), Y(InY), Width(InWidth), Height(InHeight) {
		Rect.Left = InX;
		Rect.Top = InY;
		Rect.Right = InX + InWidth;
		Rect.Bottom = InY - InHeight;
	}

	float GetX() const { return X; }
	float GetY() const { return Y; }
	float GetWidth() const { return Width; }
	float GetHeight() const { return Height; }
	void SetX(float InX) { X = InX; }
	void SetY(float InY) { Y = InY; }
	void SetWidth(float InWidth) { Width = InWidth; }
	void SetHeight(float InHeight) { Height = InHeight; }

private:
	float X, Y, Width, Height;
}; 