#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Array.h"
#include "Resource/StaticMesh/StaticMeshTypes.h"

class FArchive
{
public:
	FArchive() : bIsLoading(false), bIsSaving(false), bError(false) {}
	virtual ~FArchive() = default;

	// 기본 직렬화 함수
	virtual void Serialize(void* Data, uint64 Length) = 0;


	// 기본 타입 직렬화를 위한 연산자 오버로딩
	FArchive& operator<<(int32& Value)
	{
		Serialize(&Value, sizeof(Value));
		return *this;
	}

	FArchive& operator<<(uint32& Value)
	{
		Serialize(&Value, sizeof(Value));
		return *this;
	}

	FArchive& operator<<(float& Value)
	{
		Serialize(&Value, sizeof(Value));
		return *this;
	}

	FArchive& operator<<(bool& Value)
	{
		Serialize(&Value, sizeof(Value));
		return *this;
	}

	// std::string 직렬화
	FArchive& operator<<(std::string& Value)
	{
		int32 Length = static_cast<int32>(Value.length());
		*this << Length;

		if (IsLoading())
		{
			Value.resize(Length);
			if (Length > 0)
			{
				Serialize(&Value[0], Length);
			}
		}
		else if (IsSaving())
		{
			if (Length > 0)
			{
				Serialize((void*)Value.c_str(), Length);
			}
		}
		return *this;
	}

	FArchive& operator<<(const std::string& Value)
	{
		int32 Length = static_cast<int32>(Value.length());
		*this << Length;

		if (IsSaving())
		{
			if (Length > 0)
			{
				Serialize((void*)Value.c_str(), Length);
			}
		}
		else
		{
			SetError();
		}
		return *this;
	}

	// TArray 직렬화
	template<typename T>
	FArchive& operator<<(TArray<T>& Value)
	{
		int32 Count = static_cast<int32>(Value.Num());
		*this << Count;

		if (IsLoading())
		{
			Value.SetNum(Count);
		}

		for (int32 i = 0; i < Count; i++)
		{
			*this << Value[i];
		}

		return *this;
	}

	template<typename T>
	FArchive& operator<<(const TArray<T>& Value)
	{
		// 저장 모드에서만 사용됨
		if (IsSaving())
		{
			int32 Count = static_cast<int32>(Value.Num());
			*this << Count;

			for (int32 i = 0; i < Count; i++)
			{
				T& CastedValue = const_cast<T&>(Value[i]);
				*this << CastedValue;
			}
		}
		else
		{
			// 로딩 모드에서는 const 배열을 사용할 수 없음
			SetError();
		}

		return *this;
	}

	// TMap 직렬화
	template<typename KeyType, typename ValueType>
	FArchive& operator<<(TMap<KeyType, ValueType>& Value)
	{
		int32 Count = static_cast<int32>(Value.Num());
		*this << Count;

		if (IsLoading())
		{
			Value.Reserve(Count);
			for (int32 i = 0; i < Count; i++)
			{
				KeyType Key;
				ValueType MappedValue;
				*this << Key;
				*this << MappedValue;
				Value.Add(Key, MappedValue);
			}
		}
		else if (IsSaving())
		{
			for (auto& Pair : Value)
			{
				KeyType Key = Pair.Key;
				ValueType MappedValue = Pair.Value;
				*this << Key;
				*this << MappedValue;
			}
		}

		return *this;
	}

	// const TMap 직렬화 (저장 전용)
	template<typename KeyType, typename ValueType>
	FArchive& operator<<(const TMap<KeyType, ValueType>& Value)
	{
		if (IsSaving())
		{
			int32 Count = static_cast<int32>(Value.Num());
			*this << Count;

			for (const auto& Pair : Value)
			{
				KeyType Key = Pair.Key;
				ValueType MappedValue = Pair.Value;
				*this << Key;
				*this << MappedValue;
			}
		}
		else
		{
			SetError();
		}

		return *this;
	}

	FArchive& operator<<(FVector& Value)
	{
		*this << Value.X << Value.Y << Value.Z;
		return *this;
	}

	FArchive& operator<<(FVertexSimple& Value)
	{
		*this << Value.X << Value.Y << Value.Z << Value.R << Value.G << Value.B << Value.A << Value.U << Value.V << Value.NX << Value.NY << Value.NZ;
		return *this;
	}

	// FObjMaterialInfo 직렬화
	FArchive& operator<<(FObjMaterialInfo& Value)
	{
		*this << Value.PathFileName;
		*this << Value.DiffuseColor;
		*this << Value.DiffuseTexture;
		*this << Value.AmbientColor;
		*this << Value.SpecularColor;
		*this << Value.SpecularExponent;
		*this << Value.Opacity;
		return *this;
	}

	// FSubMeshSection 직렬화
	FArchive& operator<<(FSubMeshSection& Value)
	{
		*this << Value.indexStart;
		*this << Value.indexCount;
		*this << Value.MaterialIndex;
		*this << Value.SlotName;
		return *this;
	}

	// FStaticMesh 직렬화
	FArchive& operator<<(FStaticMesh& Value)
	{
		*this << Value.PathFileName;
		*this << Value.Vertices;
		*this << Value.Indices;
		*this << Value.MaterialInfoArray;
		*this << Value.MaterialSlotNameToIndex;
		*this << Value.Sections;
		return *this;
	}

	// const FStaticMesh 직렬화 (저장 전용)
	FArchive& operator<<(const FStaticMesh& Value)
	{
		//std::string PathName = Value.PathFileName;

		if (IsSaving())
		{
			//*this << PathName;
			*this << Value.PathFileName;
			*this << Value.Vertices;
			*this << Value.Indices;
			*this << Value.MaterialInfoArray;
			*this << Value.MaterialSlotNameToIndex;
			*this << Value.Sections;
		}
		else
		{
			// 로딩 모드에서는 const 구조체를 사용할 수 없음
			SetError();
		}

		return *this;
	}

	// 상태 확인 함수
	bool IsLoading() const { return bIsLoading; }
	bool IsSaving() const { return bIsSaving; }
	bool IsError() const { return bError; }

	// 상태 설정 함수
	void SetIsLoading(bool bValue) { bIsLoading = bValue; }
	void SetIsSaving(bool bValue) { bIsSaving = bValue; }
	void SetError() { bError = true; }

protected:
	bool bIsLoading;
	bool bIsSaving;
	bool bError;
};