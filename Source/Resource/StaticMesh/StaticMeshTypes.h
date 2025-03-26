#pragma once
#include <string>
#include "Core/Math/Vector.h"
#include "Core/Container/Map.h"
#include "Primitive/PrimitiveVertices.h"

// OBJ파일 내부 Face 정보 구조체
struct FFace {
	int32 VertexIndices[3];
	int32 UVIndices[3];
	int32 NormalIndices[3];
	std::string MaterialName;
};
	
// 재질 정보 구조체
struct FObjMaterialInfo {
	std::string PathFileName;
	FVector DiffuseColor;
	std::string DiffuseTexture;
	FVector AmbientColor;
	FVector SpecularColor;
	std::string SpecularTexture;
	FVector NormalColor;
	std::string NormalTexture;
	float SpecularExponent;
	float Opacity;

	FObjMaterialInfo() :
		DiffuseColor(1.0f, 1.0f, 1.0f),
		AmbientColor(1.0f, 1.0f, 1.0f),
		SpecularColor(1.0f, 1.0f, 1.0f),
		NormalColor(1.0f, 1.0f, 1.0f),
		SpecularExponent(0.0f),
		Opacity(1.0f) {
	}
};

// OBJ 파일 정보 구조체
struct FObjInfo {
	TArray<FVector> Vertices;
	TArray<FVector> UVs;
	TArray<FVector> Normals;
	TArray<FFace> Faces;
	TMap<std::string, FObjMaterialInfo> Materials;
};
//FIXING
struct FSubMeshSection {
	uint32 indexStart;
	uint32 indexCount;

	// Texture2DArray 인덱스
	int32 MaterialIndex;
	// 편의상 있는 에디터용
	std::string SlotName;
};

// 최종 정적 메시 구조체
struct FStaticMesh {
	std::string PathFileName;
	TArray<FVertexSimple> Vertices;
	TArray<uint32> Indices;

	//FIXING
	// 텍스처 업로드 순서 유지하기 위한 배열.
	TArray<FObjMaterialInfo> MaterialInfoArray;
	// 에디터 및 로드용 이름 접근 가능 맵.
	TMap<std::string, int32> MaterialSlotNameToIndex;
	// 메시 섹션
	TArray<FSubMeshSection> Sections;
};

