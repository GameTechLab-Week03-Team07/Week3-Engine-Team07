#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "Core/AbstractClass/Singleton.h"
#include "Core/Container/Map.h"
#include "FStaticMeshManager.h"
#include "FObjImporter.h"
#include "UStaticMesh.h"
#include "Object/ObjectFactory.h"
#include "Core/UObject/TObjectIterator.h"
#include "Resource/Texture.h"
#include "WindowsBinWriter.h"
#include "WindowsBinReader.h"
#include <iostream>
#include <fstream>

class FStaticMeshManager : public TSingleton<FStaticMeshManager>
{
public:
	// 메시 로드 및 저장
	std::shared_ptr<FStaticMesh> LoadObjStaticMeshAsset(const std::string& FilePath);

	// UStaticMesh를 로드하는 함수 (UObject를 상속받아 UUID가 생긴 상태)
	UStaticMesh* LoadObjStaticMesh(const std::string& PathFileName);

	// 메시 정보 가져오기
	std::shared_ptr<FStaticMesh> GetMesh(const std::string& MeshName);

	// UStaticMesh 인스턴스 가져오기
	UStaticMesh* GetStaticMesh(const std::string& PathFileName);

	// UStaticMesh의 텍스처 정보를 활용하여 SRV 생성하는 함수
	bool LoadTexturesFromStaticMesh(UStaticMesh* StaticMesh);

	// 메시 존재 여부 확인
	bool HasMesh(const std::string& MeshName);

	// 메시 제거
	void RemoveMesh(const std::string& MeshName);

	// 모든 메시 제거
	void ClearAllMeshes();

	// 로드된 모든 메시 이름 가져오기
	std::vector<std::string> GetAllMeshNames();

	// 로드된 메시 수 가져오기
	size_t GetMeshCount();

	// OBJ 파일을 파싱하고 바이너리로 저장
	bool ParseAndSaveToBinary(const std::string& objFilePath, const std::string& binFilePath);

	// 메시 데이터를 바이너리 파일로 저장
	bool SaveStaticMeshToBinary(const FStaticMesh& mesh, const std::string& binFilePath);

	// 바이너리 파일에서 메시 데이터 로드
	std::shared_ptr<FStaticMesh> LoadFromBinary(const std::string& binFilePath, std::shared_ptr<FStaticMesh> OutMesh);

	// 바이너리 파일이 존재하는지 확인
	bool DoesBinaryFileExist(const std::string& binFilePath);

private:
	// 메시 저장소
	TMap<std::string, std::shared_ptr<FStaticMesh>> MeshMap;
	std::mutex MeshMutex;
};
