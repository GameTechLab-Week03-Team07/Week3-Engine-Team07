#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Math/Matrix.h"
#include "Object/Window/Window.h"


class FViewport
{
public:
	FViewport(int32 InWidth, int32 InHeight)
		: Width(InWidth), Height(InHeight) {
		Windows.Add(std::make_shared<SWindow>(0, 0, Width / 2, Height / 2));
		Windows.Add(std::make_shared<SWindow>(Width / 2, 0, Width, Height / 2));
		Windows.Add(std::make_shared<SWindow>(0, Height / 2, Width / 2, Height));
		Windows.Add(std::make_shared<SWindow>(Width / 2, Height / 2, Width, Height));

		SplitterH = std::make_shared<SSplitterH>();
		SplitterV = std::make_shared<SSplitterV>();
	}

	int32 GetX() const { return X; }
	int32 GetY() const { return Y; }
	int32 GetWidth() const { return Width; }
	int32 GetHeight() const { return Height; }
	std::shared_ptr<SWindow> GetWindow(int index) const { return Windows[index]; }

	std::shared_ptr<SSplitterH> SplitterH;
	std::shared_ptr<SSplitterV> SplitterV;

private:
	int32 X, Y, Width, Height;
	TArray<std::shared_ptr<SWindow>> Windows;
}; 

class FPerspectiveViewport : public FViewport
{
public:
	FPerspectiveViewport(int32 InWidth, int32 InHeight)
		: FViewport(InWidth, InHeight) {
	}

	FMatrix PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane) {
		return FMatrix::PerspectiveFovLH(FieldOfView, AspectRatio, NearPlane, FarPlane);
	}
};

class FOrthogonalViewport : public FViewport
{

};