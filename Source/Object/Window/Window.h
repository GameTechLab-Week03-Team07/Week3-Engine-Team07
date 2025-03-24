#pragma once
#include "Core/Math/Vector.h"
#include "Core/UObject/Object.h"

struct FRect
{
	float Left, Top, Right, Bottom;

	float GetWidth() const { return Right - Left; }
	float GetHeight() const { return Top - Bottom; }
};

class SWindow : public UObject
{
public:
	FRect Rect;

	bool IsHover(FVector coord) const {
		return coord.X >= Rect.Left && coord.X <= Rect.Right && coord.Y <= Rect.Top && coord.Y >= Rect.Bottom;
	}

public:
	SWindow() = default;
	SWindow(float Left, float Top, float Right, float Bottom)
	{
		Rect.Left = Left;
		Rect.Top = Top;
		Rect.Right = Right;
		Rect.Bottom = Bottom;
	};

	~SWindow() = default;
};

class SSplitter : public SWindow
{
public:
	std::shared_ptr<SWindow> SideLT; // Left or Top
	std::shared_ptr<SWindow> SideRB; // Right or Bottom

};

class SSplitterH : public SSplitter
{
public:
	SSplitterH() = default;
	SSplitterH(float Left, float Top, float Right, float Bottom)
		: SSplitter()
	{
		Rect.Left = Left;
		Rect.Top = Top;
		Rect.Right = Right;
		Rect.Bottom = Bottom;
	}
};

class SSplitterV : public SSplitter
{
public:
	SSplitterV() = default;
	SSplitterV(float Left, float Top, float Right, float Bottom)
		: SSplitter()
	{
		Rect.Left = Left;
		Rect.Top = Top;
		Rect.Right = Right;
		Rect.Bottom = Bottom;
	}
};