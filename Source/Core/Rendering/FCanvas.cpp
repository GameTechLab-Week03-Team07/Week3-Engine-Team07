//#include "FCanvas.h"
//
//void FCanvas::Init(ID2D1RenderTarget* InRenderTarget, IDWriteTextFormat* InTextFormat)
//{
//	RenderTarget = InRenderTarget;
//	TextFormat = InTextFormat;
//}
//
//void FCanvas::DrawText(const std::wstring& Text, float x, float y, D2D1::ColorF Color)
//{
//	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> Brush;
//    RenderTarget->CreateSolidColorBrush(Color, &Brush);
//
//	D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 300, y + 50);
//	RenderTarget->DrawTextW(Text.c_str(),
//		(UINT32)Text.length(),
//		TextFormat,
//		&layoutRect,
//		Brush.Get()
//	);
//	
//
//}
//
//void FCanvas::Begin()
//{
//	RenderTarget->BeginDraw();
//}
//
//void FCanvas::End()
//{
//	RenderTarget->EndDraw();
//}
