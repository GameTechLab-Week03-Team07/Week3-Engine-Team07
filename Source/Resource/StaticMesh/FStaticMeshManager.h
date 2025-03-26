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
	bool LoadTexturesFromStaticMesh(UStaticMesh* StaticMesh)
	{
		if (!StaticMesh || !StaticMesh->StaticMeshAsset)
			return false;

		bool bSuccess = true;

		// StaticMesh의 모든 재질 순회 - 추후 확산 텍스처 맵 이외 다른 처리도 필요할 듯
		const auto& MaterialArray = StaticMesh->StaticMeshAsset->MaterialInfoArray;
		for (int32 i = 0; i < MaterialArray.Num(); ++i) {
			const auto& Material = MaterialArray[i];
			// diffuse
			if (!Material.DiffuseTexture.empty())
			{
				// 텍스처 경로 구성
				// Contents/StaticMesh 폴더 내의 상대 경로로 변환
				FString TexturePath = "Contents/StaticMesh/";
				// ?? 이거 이어붙이면 왜 안되지??
				TexturePath += Material.DiffuseTexture.c_str();

				// 텍스처 이름으로 MaterialName 사용
				//FString TextureName = Material.PathFileName + "_Diffuse";

				// 텍스처 이름으로 MaterialArray 이름 사용
				FString arrayName = "StaticMeshTextureArray_" + StaticMesh->GetAssetPathFileName();

				TArray<FString> texturePaths;

				const auto& MaterialArray = StaticMesh->StaticMeshAsset->MaterialInfoArray;

				for (int i = 0; i < MaterialArray.Num(); ++i) {
					const auto& material = MaterialArray[i];

					if (!material.DiffuseTexture.empty())
					{
						// 상대 경로를 문자열로 만들기
						std::string fullPath = "Contents/StaticMesh/" + material.DiffuseTexture;

						// std::string → FString 변환
						FString pathFStr = fullPath;

						// 배열에 추가
						texturePaths.Add(pathFStr);
					}

				}
				// 텍스처 로드 및 SRV 생성
				try {
					std::shared_ptr<FTexture> TextureImage = FTexture::Load(texturePaths, arrayName);
					if (TextureImage) {
						TextureImage->CreateShaderResourceView();
						// 성공적으로 로드된 텍스처 정보 출력 (디버깅용)
						std::cout << "Loaded texture: " << arrayName << " from " << TexturePath << std::endl;
					}
					else {
						std::cerr << "Failed to load texture: " << arrayName << std::endl;
						bSuccess = false;
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Exception loading texture: " << e.what() << std::endl;
					bSuccess = false;
				}
			}
			// normal(bump)
			if (!Material.NormalTexture.empty())
			{
				// 텍스처 경로 구성
				// Contents/StaticMesh 폴더 내의 상대 경로로 변환
				FString TexturePath = "Contents/StaticMesh/";
				// ?? 이거 이어붙이면 왜 안되지??
				TexturePath += Material.NormalTexture.c_str();

				// 텍스처 이름으로 MaterialName 사용
				//FString TextureName = Material.PathFileName + "_Diffuse";

				// 텍스처 이름으로 MaterialArray 이름 사용
				FString arrayName = "StaticMeshNormalTextureArray_" + StaticMesh->GetAssetPathFileName();

				TArray<FString> texturePaths;

				const auto& MaterialArray = StaticMesh->StaticMeshAsset->MaterialInfoArray;

				for (int i = 0; i < MaterialArray.Num(); ++i) {
					const auto& material = MaterialArray[i];

					if (!material.NormalTexture.empty())
					{
						// 상대 경로를 문자열로 만들기
						std::string fullPath = "Contents/StaticMesh/" + material.NormalTexture;

						// std::string → FString 변환
						FString pathFStr = fullPath;

						// 배열에 추가
						texturePaths.Add(pathFStr);
					}

				}
				// 텍스처 로드 및 SRV 생성
				try {
					std::shared_ptr<FTexture> TextureImage = FTexture::Load(texturePaths, arrayName);
					if (TextureImage) {
						TextureImage->CreateShaderResourceView();
						// 성공적으로 로드된 텍스처 정보 출력 (디버깅용)
						std::cout << "Loaded texture: " << arrayName << " from " << TexturePath << std::endl;
					}
					else {
						std::cerr << "Failed to load texture: " << arrayName << std::endl;
						bSuccess = false;
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Exception loading texture: " << e.what() << std::endl;
					bSuccess = false;
				}
			}
			// specular
			if (!Material.SpecularTexture.empty())
			{
				// 텍스처 경로 구성
				// Contents/StaticMesh 폴더 내의 상대 경로로 변환
				FString TexturePath = "Contents/StaticMesh/";
				// ?? 이거 이어붙이면 왜 안되지??
				TexturePath += Material.SpecularTexture.c_str();

				// 텍스처 이름으로 MaterialName 사용
				//FString TextureName = Material.PathFileName + "_Diffuse";

				// 텍스처 이름으로 MaterialArray 이름 사용
				FString arrayName = "StaticMeshSpecularTextureArray_" + StaticMesh->GetAssetPathFileName();

				TArray<FString> texturePaths;

				const auto& MaterialArray = StaticMesh->StaticMeshAsset->MaterialInfoArray;

				for (int i = 0; i < MaterialArray.Num(); ++i) {
					const auto& material = MaterialArray[i];

					if (!material.SpecularTexture.empty())
					{
						// 상대 경로를 문자열로 만들기
						std::string fullPath = "Contents/StaticMesh/" + material.SpecularTexture;

						// std::string → FString 변환
						FString pathFStr = fullPath;

						// 배열에 추가
						texturePaths.Add(pathFStr);
					}

				}
				// 텍스처 로드 및 SRV 생성
				try {
					std::shared_ptr<FTexture> TextureImage = FTexture::Load(texturePaths, arrayName);
					if (TextureImage) {
						TextureImage->CreateShaderResourceView();
						// 성공적으로 로드된 텍스처 정보 출력 (디버깅용)
						std::cout << "Loaded texture: " << arrayName << " from " << TexturePath << std::endl;
					}
					else {
						std::cerr << "Failed to load texture: " << arrayName << std::endl;
						bSuccess = false;
					}
				}
				catch (const std::exception& e) {
					std::cerr << "Exception loading texture: " << e.what() << std::endl;
					bSuccess = false;
				}
			}

		}

		return bSuccess;
	}

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
