#pragma once
#include "Core/Math/Vector.h"
#include "Core/UObject/Object.h"

struct FRect
{
	float Left, Top, Right, Bottom;
};

class SWindow : public UObject
{
public:
	FRect Rect;
	bool IsHover(FVector coord) const;
	virtual void Render() {};

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
	float SplitRatio = 0.5f; // 스플릿 비율
	static constexpr float DefaultSplitterWidth = 0.001f;

	virtual void OnDrag(float Delta) = 0; // 드래그 이벤트
};

class SSplitterH : public SSplitter
{
public:
	void Render() override
	{
		if (SideLT) SideLT->Render();
		if (SideRB) SideRB->Render();
	}

	void OnDrag(float Delta) override
	{
		SplitRatio += Delta;
		if (SplitRatio < 0.1f) SplitRatio = 0.1f;
		if (SplitRatio > 0.9f) SplitRatio = 0.9f;
	}
};

class SSplitterV : public SSplitter
{
public:
	void Render() override
	{
		if (SideLT) SideLT->Render();
		if (SideRB) SideRB->Render();
	}

	void OnDrag(float Delta) override
	{
		SplitRatio += Delta;
		if (SplitRatio < 0.1f) SplitRatio = 0.1f;
		if (SplitRatio > 0.9f) SplitRatio = 0.9f;
	}

};