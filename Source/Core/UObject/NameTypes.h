#pragma once
#include "Core/HAL/PlatformType.h"

class FString;


class FName
{
	friend struct FNameHelper;

	uint32 DisplayIndex;    // 원본 문자열의 Hash
	uint32 ComparisonIndex; // 비교시 사용되는 Hash

public:
	FName() : DisplayIndex(0), ComparisonIndex(0) {}
	FName(const WIDECHAR* Name);
	FName(const ANSICHAR* Name);
	FName(const FString& Name);

	FString ToString() const;
	uint32 GetDisplayIndex() const { return DisplayIndex; }
	uint32 GetComparisonIndex() const { return ComparisonIndex; }

	bool operator==(const FName& Other) const;
};
// FName.h 또는 FName이 정의된 곳의 하단부에 추가
#pragma once
#include <functional> // std::hash 사용을 위해 필요

// FName 클래스 정의가 끝난 뒤에 추가할 것!
namespace std
{
	template <>
	struct hash<FName>
	{
		std::size_t operator()(const FName& Name) const noexcept
		{
			return static_cast<std::size_t>(Name.GetComparisonIndex());
		}
	};
}