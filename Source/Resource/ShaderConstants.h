#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Vector.h"
#include "Core/HAL/PlatformType.h"


struct alignas(16) FMatIndexConstantsComponentData
{
	int MatIndex;
};


//상수버퍼로 객체의 정보를 넣을 구조체
struct alignas(16) FConstantsComponentData
{
public:
	FMatrix MVP;
	FVector4 Color;
	// true인 경우 Vertex Color를 사용하고, false인 경우 Color를 사용합니다.
	FVector4 UUIDColor;
	uint32 bUseVertexColor;
	FVector Padding;
};