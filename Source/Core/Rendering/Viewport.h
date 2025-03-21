#pragma once
#include "Core/HAL/PlatformType.h"

class FViewport
{
public:
	FViewport(int32 InX, int32 InY, int32 InWidth, int32 InHeight)
		: X(InX), Y(InY), Width(InWidth), Height(InHeight) {
	}

	int32 GetX() const { return X; }
	int32 GetY() const { return Y; }
	int32 GetWidth() const { return Width; }
	int32 GetHeight() const { return Height; }

private:
	int32 X;
	int32 Y;
	int32 Width;
	int32 Height;
};
