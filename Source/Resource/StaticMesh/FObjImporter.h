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
			// OBJ semantics https://en.wikipedia.org/wiki/Wavefront_.obj_file
			if (prefix == "v") {
				// 정점 위치
				FVector vertex;
				iss >> vertex.X >> vertex.Y >> vertex.Z;
				OutObjInfo.Vertices.Add(vertex);
			}
			else if (prefix == "vt") {
				// 텍스처 좌표
				FVector uv;
				iss >> uv.X >> uv.Y;
				// OBJ는 V 좌표가 아래에서 위로 증가하므로 Y 좌표를 뒤집음
				uv.Y = 1.0f - uv.Y;
				OutObjInfo.UVs.Add(uv);
			}
			else if (prefix == "vn") {
				// 법선 벡터
				FVector normal;
				iss >> normal.X >> normal.Y >> normal.Z;
				OutObjInfo.Normals.Add(normal);
			}
			else if (prefix == "f") {
				// 면 정의
				std::vector<std::string> vertexDataList;
				std::string vertexData;

				// 면의 모든 정점 데이터 수집
				while (iss >> vertexData) {
					vertexDataList.push_back(vertexData);
				}

				// 삼각형화 - 3개 이상의 정점이 있는 경우 처리
				if (vertexDataList.size() >= 3) {
					// 팬(fan) 삼각형화 방식 사용
					for (size_t i = 0; i < vertexDataList.size() - 2; ++i) {
						FFace face;
						face.MaterialName = currentMaterial;

						// 첫 번째 정점은 항상 중심점
						ParseVertexData(vertexDataList[0], face, 0);

						// 나머지 두 정점으로 삼각형 형성
						ParseVertexData(vertexDataList[i + 1], face, 1);
						ParseVertexData(vertexDataList[i + 2], face, 2);

						OutObjInfo.Faces.Add(face);
					}
				}
			}
			// 머티리얼 파일이 존재하는 경우 처리
			else if (prefix == "mtllib") {
				// 재질 라이브러리 파일
				iss >> mtlLibPath;
				// 경로 조합 현재 Contents/StaticMesh 하위로 모두 집어넣음
				std::string directory = GetDirectoryPath(FilePath);
				ParseMtlFile(directory + "/" + mtlLibPath, OutObjInfo);
			}
			else if (prefix == "usemtl") {
				// 현재 사용 중인 재질
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
			else if (prefix == "map_Bump") {
				// NormalTexture
				iss >> currentMaterial.NormalTexture;
			}
			else if (prefix == "map_Ks") {
				// SpecularTexture
				iss >> currentMaterial.SpecularTexture;
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
		// Material 슬롯 등록
		int matIndex = 0;
		for (const auto& [name, info] : ObjInfo.Materials) {
			OutMesh.MaterialSlotNameToIndex[name] = matIndex++;
			OutMesh.MaterialInfoArray.Add(info);
		}

		// 섹션별 face 그룹화
		//FIXME : unordered_map, vector 사용자 정의 자료구조로 변경.
		std::unordered_map<std::string, std::vector<const FFace*>> FacesByMaterial;
		for (const FFace& face : ObjInfo.Faces) {
			FacesByMaterial[face.MaterialName].push_back(&face);
		}

		// 공통 정점 캐싱
		std::unordered_map<std::string, int> vertexCache;
		int nextVertexIndex = 0;

		// 섹션 단위로 처리
		for (const auto& [materialName, faceList] : FacesByMaterial) {
			int indexStart = OutMesh.Indices.Num();

			for (const FFace* face : faceList) {
				for (int i = 0; i < 3; i++) {
					FVertexSimple vertex;

					// 위치
					if (face->VertexIndices[i] >= 0 && face->VertexIndices[i] < ObjInfo.Vertices.Num()) {
						const FVector& pos = ObjInfo.Vertices[face->VertexIndices[i]];
						vertex.X = pos.X; vertex.Y = pos.Y; vertex.Z = pos.Z;
					}

					// UV
					if (face->UVIndices[i] >= 0 && face->VertexIndices[i] < ObjInfo.Vertices.Num()) {
						const FVector& uv = ObjInfo.UVs[face->UVIndices[i]];
						vertex.U = uv.X; vertex.V = uv.Y;
					}
					else {
						vertex.U = 0.0f; vertex.V = 0.0f;
					}

					// 법선
					if (face->NormalIndices[i] >= 0 && face->NormalIndices[i] < ObjInfo.Normals.Num()) {
						const FVector& normal = ObjInfo.Normals[face->NormalIndices[i]];
						vertex.NX = 0.0f; vertex.NY = 1.0f; vertex.NX = 0.0f;
					}
					else {
						vertex.NX = 0.0f; vertex.NY = 1.0f; vertex.NX = 0.0f;
					}

					//색상 (머티리얼 또는 랜덤)
					if (!materialName.empty() && ObjInfo.Materials.Contains(materialName)) {
						const FObjMaterialInfo& material = ObjInfo.Materials[materialName];
						vertex.R = material.DiffuseColor.X;
						vertex.G = material.DiffuseColor.Y;
						vertex.B = material.DiffuseColor.Z;
						vertex.A = material.Opacity;
					}
					else {
						FVector color = GetColorFromPalette(OutMesh.Vertices.Num() / 3);
						vertex.R = color.X; vertex.G = color.Y; vertex.B = color.Z; vertex.A = 1.0f;
					}

					// 중복 정점 체크
					std::string key = CreateVertexKey(vertex);
					if (vertexCache.find(key) != vertexCache.end()) {
						OutMesh.Indices.Add(vertexCache[key]);
					}
					else {
						OutMesh.Vertices.Add(vertex);
						OutMesh.Indices.Add(nextVertexIndex);
						vertexCache[key] = nextVertexIndex++;
					}
				}
			}
			int indexCount = OutMesh.Indices.Num() - indexStart;

			FSubMeshSection section;
			section.indexStart = indexStart;
			section.indexCount = indexCount;
			section.SlotName = materialName;
			section.MaterialIndex = OutMesh.MaterialSlotNameToIndex[materialName];

			OutMesh.Sections.Add(section);
		}

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
