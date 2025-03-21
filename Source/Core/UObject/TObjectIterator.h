#pragma once
#include "Core/UObject/Object.h"
#include "Core/Engine.h"
#include "Core/Container/Array.h"

template<typename T>
	requires std::derived_from<T, UObject>
class TObjectIterator
{
private:
	size_t CurrentIndex;
	TArray<T*> FilteredObjects;

public:
	TObjectIterator() : CurrentIndex(0)
	{
		// UEngine의 GObjects에서 타입 T에 해당하는 객체들만 필터링
		for (const auto& Pair : UEngine::Get().GObjects)
		{
			if (T* CastedObject = Cast<T>(Pair.Value.get()))
			{
				FilteredObjects.Add(CastedObject);
			}
		}
	}

	// 현재 객체 반환
	T* operator*() const
	{
		if (CurrentIndex < FilteredObjects.Num())
		{
			return FilteredObjects[CurrentIndex];
		}
		return nullptr;
	}

	// 다음 객체로 이동
	TObjectIterator& operator++()
	{
		++CurrentIndex;
		return *this;
	}

	// 후위 증가 연산자
	TObjectIterator operator++(int)
	{
		TObjectIterator temp = *this;
		++(*this);
		return temp;
	}

	// 반복자 유효성 검사
	operator bool() const
	{
		return CurrentIndex < FilteredObjects.Num();
	}

	// 반복자 비교 연산자
	bool operator==(const TObjectIterator& Other) const
	{
		return CurrentIndex == Other.CurrentIndex;
	}

	bool operator!=(const TObjectIterator& Other) const
	{
		return !(*this == Other);
	}
};
