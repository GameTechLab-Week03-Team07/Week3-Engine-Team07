#pragma once

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include "Core/Container/Array.h"
//#include "Core/Container/String.h"
#include "StaticMeshTypes.h"

namespace fs = std::filesystem;

class FObjImporter {
public:
	// 색상 정보가 없는 경우를 위한 팔레스 및 가벼운 랜덤 지정 함수
	const std::vector<FVector> colorPalette = {
	{1.0f, 0.0f, 0.0f}, // 빨강
	{0.0f, 1.0f, 0.0f}, // 초록
	{0.0f, 0.0f, 1.0f}, // 파랑
	{1.0f, 1.0f, 0.0f}, // 노랑
	{1.0f, 0.0f, 1.0f}, // 마젠타
	{0.0f, 1.0f, 1.0f}, // 시안
	{1.0f, 0.5f, 0.0f}, // 주황
	{0.5f, 0.0f, 0.5f}, // 보라
	{0.0f, 0.5f, 0.0f}, // 다크 그린
	{0.5f, 0.5f, 0.0f}, // 올리브
	{0.0f, 0.0f, 0.5f}, // 네이비
	{0.5f, 0.0f, 0.0f}  // 마룬
	};

	FVector GetColorFromPalette(int index) {
		return colorPalette[index % colorPalette.size()];
	}
	// OBJ 파일 파싱
	bool ParseObjFile(const std::string& FilePath, FObjInfo& OutObjInfo) {
		std::ifstream file(FilePath);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << FilePath << std::endl;
			return false;
		}

		std::string line;
		std::string currentMaterial;
		std::string mtlLibPath;

		// 로컬 저장소
		TArray<FVector> tempVertices;
		TArray<FVector> tempUVs;
		TArray<FVector> tempNormals;

		while (std::getline(file, line)) {
			size_t commentPos = line.find('#');
			if (commentPos != std::string::npos) {
				line = line.substr(0, commentPos);
			}

			line = Trim(line);
			if (line.empty()) continue;

			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "v") {
				FVector vertex;
				iss >> vertex.X >> vertex.Y >> vertex.Z;
				tempVertices.Add(vertex);
			}
			else if (prefix == "vt") {
				FVector uv;
				iss >> uv.X >> uv.Y;
				uv.Y = 1.0f - uv.Y;
				tempUVs.Add(uv);
			}
			else if (prefix == "vn") {
				FVector normal;
				iss >> normal.X >> normal.Y >> normal.Z;
				tempNormals.Add(normal);
			}
			else if (prefix == "f") {
				std::vector<std::string> vertexDataList;
				std::string vertexData;

				while (iss >> vertexData) {
					vertexDataList.push_back(vertexData);
				}

				if (vertexDataList.size() >= 3) {
					for (size_t i = 0; i < vertexDataList.size() - 2; ++i) {
						std::string materialName = currentMaterial.empty() ? "__default" : currentMaterial;

						// 섹션 찾기 또는 새로 만들기
						if (!OutObjInfo.Sections.Contains(materialName)) {
							OutObjInfo.Sections[materialName] = TArray<FFace>();
						}

						FFace face;
						face.MaterialName = materialName;

						ParseVertexData(vertexDataList[0], face, 0);
						ParseVertexData(vertexDataList[i + 1], face, 1);
						ParseVertexData(vertexDataList[i + 2], face, 2);

						OutObjInfo.Sections[materialName].Add(face);
					}
				}
			}
			else if (prefix == "mtllib") {
				iss >> mtlLibPath;
				std::string directory = GetDirectoryPath(FilePath);
				ParseMtlFile(directory + "/" + mtlLibPath, OutObjInfo);
			}
			else if (prefix == "usemtl") {
				iss >> currentMaterial;
			}
		}

		file.close();
		return true;
	}

	// 정점 데이터 파싱 헬퍼 함수
	void ParseVertexData(const std::string& vertexData, FFace& face, int vertexIndex) {
		std::istringstream vertexStream(vertexData);
		std::string indexStr;

		// 정점 인덱스
		std::getline(vertexStream, indexStr, '/');
		face.VertexIndices[vertexIndex] = std::stoi(indexStr) - 1; // OBJ는 1부터 인덱싱

		// UV 인덱스 (있을 경우)
		if (std::getline(vertexStream, indexStr, '/') && !indexStr.empty()) {
			face.UVIndices[vertexIndex] = std::stoi(indexStr) - 1;
		}
		else {
			face.UVIndices[vertexIndex] = -1;
		}

		// 법선 인덱스 (있을 경우)
		if (std::getline(vertexStream, indexStr, '/') && !indexStr.empty()) {
			face.NormalIndices[vertexIndex] = std::stoi(indexStr) - 1;
		}
		else {
			face.NormalIndices[vertexIndex] = -1;
		}
	}


	// MTL 파일 파싱
	bool ParseMtlFile(const std::string& FilePath, FObjInfo& OutObjInfo) {
		std::ifstream file(FilePath);
		if (!file.is_open()) {
			std::cerr << "Failed to open material file: " << FilePath << std::endl;
			return false;
		}

		std::string line;
		FObjMaterialInfo currentMaterial;
		std::string currentMaterialName;

		while (std::getline(file, line)) {
			// 주석 제거
			size_t commentPos = line.find('#');
			if (commentPos != std::string::npos) {
				line = line.substr(0, commentPos);
			}

			// 공백 제거
			line = Trim(line);
			if (line.empty()) continue;

			std::istringstream iss(line);
			std::string prefix;
			iss >> prefix;

			if (prefix == "newmtl") {
				// 이전 재질이 있으면 저장
				if (!currentMaterialName.empty()) {
					OutObjInfo.Materials[currentMaterialName] = currentMaterial;
				}

				// 새 재질 시작
				iss >> currentMaterialName;
				currentMaterial = FObjMaterialInfo();
				currentMaterial.PathFileName = currentMaterialName;
			}
			else if (prefix == "Kd") {
				// Diffuse Color
				iss >> currentMaterial.DiffuseColor.X >> currentMaterial.DiffuseColor.Y >> currentMaterial.DiffuseColor.Z;
			}
			else if (prefix == "Ka") {
				// Ambient Color
				iss >> currentMaterial.AmbientColor.X >> currentMaterial.AmbientColor.Y >> currentMaterial.AmbientColor.Z;
			}
			else if (prefix == "Ks") {
				// Specular Color
				iss >> currentMaterial.SpecularColor.X >> currentMaterial.SpecularColor.Y >> currentMaterial.SpecularColor.Z;
			}
			else if (prefix == "Ns") {
				// Specular Exponent
				iss >> currentMaterial.SpecularExponent;
			}
			else if (prefix == "d" || prefix == "Tr") {
				// Transparency
				iss >> currentMaterial.Opacity;
			}
			else if (prefix == "map_Kd") {
				// DiffuseTexture
				iss >> currentMaterial.DiffuseTexture;
			}
		}

		// 마지막 재질 저장
		if (!currentMaterialName.empty()) {
			OutObjInfo.Materials[currentMaterialName] = currentMaterial;
		}

		file.close();
		return true;
	}

	// OBJ Raw 데이터를 엔진에서 사용할 수 있는 데이터로 변환
	bool ConvertToStaticMesh(const FObjInfo& ObjInfo, FStaticMesh& OutMesh) {
		std::unordered_map<std::string, int> materialToSectionIndex;
		// 섹션별 정점 캐시
		std::unordered_map<std::string, std::unordered_map<std::string, uint32>> vertexCaches;

		for (const auto& section : ObjInfo.Sections) {
			std::string materialName = section.Key;
			const TArray <FFace> &faceList = section.Value;

			if (materialName.empty()) {
				materialName = "__default";
			}
			// 섹션 생성 or 획득
			int sectionIndex;
			if (materialToSectionIndex.find(materialName) == materialToSectionIndex.end()) {
				sectionIndex = static_cast<int>(OutMesh.sections.Num());
				FMeshSection section;
				section.MaterialSlotName = materialName;
				OutMesh.sections.Add(section);
				materialToSectionIndex[materialName] = sectionIndex;
			}
			else {
				sectionIndex = materialToSectionIndex[materialName];
			}

			FMeshSection& section = OutMesh.sections[sectionIndex];
			auto& vertexCache = vertexCaches[materialName];

			for (const FFace& face : faceList) {
				for (int i = 0; i < 3; i++) {
					FVertexSimple vertex;

					// 정점 위치 설정
					if (face.VertexIndices[i] >= 0 && face.VertexIndices[i][i] < ObjInfo.Vertices.Num()) {

						vertex.X = ObjInfo.Vertices[section.Indices[i]].X;
						vertex.Y = ObjInfo.Vertices[section.Indices[i]].Y;
						vertex.Z = ObjInfo.Vertices[section.Indices[i]].Z;
					}
					// UV 좌표 설정
					if (face.UVIndices[i] >= 0 && face.UVIndices[i] < ObjInfo.UVs.Num()) {
						vertex.U = ObjInfo.UVs[face.UVIndices[i]].X;
						vertex.V = ObjInfo.UVs[face.UVIndices[i]].Y;
					}
					else {
						// UV 좌표가 없는 경우 기본값 설정
						vertex.U = 0.0f;
						vertex.V = 0.0f;
					}
					// 법선 벡터 설정
					if (face.NormalIndices[i] >= 0 && face.NormalIndices[i] < ObjInfo.Normals.Num()) {
						vertex.NX = ObjInfo.Normals[face.NormalIndices[i]].X;
						vertex.NY = ObjInfo.Normals[face.NormalIndices[i]].Y;
						vertex.NZ = ObjInfo.Normals[face.NormalIndices[i]].Z;
					}
					else {
						// 법선이 없는 경우 기본값 설정 (위쪽 방향)
						vertex.NX = 0.0f;
						vertex.NY = 1.0f;
						vertex.NZ = 0.0f;
					}
					// 재질에 따른 색상 설정
					if (!face.MaterialName.empty() && ObjInfo.Materials.Contains(face.MaterialName)) {
						const FObjMaterialInfo& material = ObjInfo.Materials[face.MaterialName];
						vertex.R = material.DiffuseColor.X;
						vertex.G = material.DiffuseColor.Y;
						vertex.B = material.DiffuseColor.Z;
						vertex.A = material.Opacity;
					}
					else {
						// 색상 정보가 없는 경우 기본 색상 설정
						FVector color = GetColorFromPalette(section.vertices.Num() / 3);
						vertex.R = color.X;
						vertex.G = color.Y;
						vertex.B = color.Z;
						vertex.A = 1.0f;
					}

					// 정점 중복 제거를 위한 키 생성
					std::string vertexKey = CreateVertexKey(vertex);

					// 이미 처리된 정점인지 확인
					if (vertexCache.find(vertexKey) != vertexCache.end()) {
						section.Indices.Add(vertexCache[vertexKey]);
					}
					else {
						section.vertices.Add(vertex);
						uint32 newIndex = static_cast<uint32>(section.vertices.Num() - 1);
						section.Indices.Add(newIndex);
						vertexCache[vertexKey] = newIndex;
					}
				}
			}
		}

		// 재질 복사
		OutMesh.Materials = ObjInfo.Materials;

		return true;
	}

	// 전체 임포트 프로세스
	bool ImportObjFile(const std::string& FilePath, FStaticMesh& OutMesh) {
		FObjInfo objInfo;
		// 현재 Contents/StaticMesh 하위로 .obj .mtl 및 이미지 파일을 전부 집어넣음 - 추후 구조화 필요할 듯
		fs::path projectRoot = fs::current_path();
		fs::path absolutePath = projectRoot / TEXT("Contents") / TEXT("StaticMesh") / FilePath;

		if (!ParseObjFile(absolutePath.string(), objInfo)) {
			return false;
		}

		OutMesh.PathFileName = FilePath;
		return ConvertToStaticMesh(objInfo, OutMesh);
	}

private:
	// 문자열 앞뒤 공백 제거
	std::string Trim(const std::string& str) {
		size_t first = str.find_first_not_of(" \t\n\r\f\v");
		if (first == std::string::npos) return "";
		size_t last = str.find_last_not_of(" \t\n\r\f\v");
		return str.substr(first, (last - first + 1));
	}

	std::string GetDirectoryPath(const std::string& filePath) {
		size_t lastSlash = filePath.find_last_of("/\\");
		if (lastSlash != std::string::npos) {
			return filePath.substr(0, lastSlash);
		}
		return "";
	}

	// 정점 중복 제거를 위한 고유 키 생성
	std::string CreateVertexKey(const FVertexSimple& vertex) {
		// 부동소수점 정밀도 문제를 줄이기 위해 반올림
		std::stringstream ss;
		ss << std::fixed << std::setprecision(6); // 정밀도 조정
		ss << vertex.X << "_" << vertex.Y << "_" << vertex.Z << "_"
			<< vertex.NX << "_" << vertex.NY << "_" << vertex.NZ << "_"
			<< vertex.U << "_" << vertex.V;
		return ss.str();
	}
};
