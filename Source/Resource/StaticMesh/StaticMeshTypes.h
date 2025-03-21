#pragma once
#include <string>
#include "Core/Math/Vector.h"
#include "Core/Container/Map.h"
#include "Primitive/PrimitiveVertices.h"

// OBJ파일 내부 Face 정보 구조체
struct FFace {
	int VertexIndices[3];
	int UVIndices[3];
	int NormalIndices[3];
	std::string MaterialName;
};

// 재질 정보 구조체
struct FObjMaterialInfo {
	std::string PathFileName;
	FVector DiffuseColor;
	std::string DiffuseTexture;
	FVector AmbientColor;
	FVector SpecularColor;
	float SpecularExponent;
	float Opacity;

	FObjMaterialInfo() :
		DiffuseColor(1.0f, 1.0f, 1.0f),
		AmbientColor(1.0f, 1.0f, 1.0f),
		SpecularColor(1.0f, 1.0f, 1.0f),
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

// 최종 정적 메시 구조체
struct FStaticMesh {
	std::string PathFileName;
	TArray<FVertexSimple> Vertices;
	TArray<uint32> Indices;
	TMap<std::string, FObjMaterialInfo> Materials;
};