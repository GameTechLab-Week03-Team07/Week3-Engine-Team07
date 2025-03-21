#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include "FObjImporter.h"
#include "UStaticMesh.h"
#include "Core/AbstractClass/Singleton.h" 
#include "Object/ObjectFactory.h"
#include "Core/UObject/TObjectIterator.h"
#include "Resource/Texture.h"

// 메시 매니저 클래스 (싱글톤 패턴)
class FStaticMeshManager : public TSingleton<FStaticMeshManager>
{
public:
	// 메시 로드 및 저장
	std::shared_ptr<FStaticMesh> LoadObjStaticMeshAsset(const std::string& FilePath)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);

		// 이미 로드된 메시인지 확인
		if (MeshMap.Find(FilePath))
		{
			return *MeshMap.Find(FilePath); // 이미 로드됨
		}

		// 새 메시 생성
		std::shared_ptr<FStaticMesh> newMesh = std::make_shared<FStaticMesh>();

		// OBJ 파일 임포트
		FObjImporter importer;
		if (!importer.ImportObjFile(FilePath, *newMesh))
		{
			return nullptr; // 로드 실패
		}

		// 메시 맵에 저장
		MeshMap[FilePath] = newMesh;
		return newMesh;
	}

	UStaticMesh* LoadObjStaticMesh(const std::string& PathFileName)
	{
		// 이미 로드된 UStaticMesh 찾기
		for (TObjectIterator<UStaticMesh> It; It; ++It)
		{
			UStaticMesh* StaticMesh = *It;
			if (StaticMesh->GetAssetPathFileName() == PathFileName)
				return StaticMesh;
		}

		// 없으면 새로 로드
		std::shared_ptr<FStaticMesh> Asset = LoadObjStaticMeshAsset(PathFileName);
		if (!Asset)
			return nullptr;

		UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
		StaticMesh->SetStaticMeshAsset(Asset);

		// 텍스처 로드 및 SRV 생성
		LoadTexturesFromStaticMesh(StaticMesh);

		return StaticMesh;
	}

	// 메시 가져오기
	std::shared_ptr<FStaticMesh> GetMesh(const std::string& MeshName)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);

		auto res = MeshMap.Find(MeshName);
		if (res)
		{
			return *res;
		}

		return nullptr; // 메시를 찾을 수 없음
	}

	// UStaticMesh 인스턴스 가져오기
	UStaticMesh* GetStaticMesh(const std::string& PathFileName)
	{
		// 먼저 TObjectIterator로 이미 존재하는 UStaticMesh 찾기
		for (TObjectIterator<UStaticMesh> It; It; ++It)
		{
			UStaticMesh* StaticMesh = *It;
			if (StaticMesh->GetAssetPathFileName() == PathFileName)
				return StaticMesh;
		}

		// 없으면 로드된 FStaticMesh 에셋이 있는지 확인
		std::shared_ptr<FStaticMesh> Asset = GetMesh(PathFileName);
		if (Asset)
		{
			// FStaticMesh는 있지만 UStaticMesh는 없는 경우 새로 생성
			UStaticMesh* StaticMesh = FObjectFactory::ConstructObject<UStaticMesh>();
			StaticMesh->SetStaticMeshAsset(Asset);
			return StaticMesh;
		}

		// 둘 다 없으면 새로 로드
		return LoadObjStaticMesh(PathFileName);
	}

	// UStaticMesh의 텍스처 정보를 활용하여 SRV 생성
	bool LoadTexturesFromStaticMesh(UStaticMesh* StaticMesh)
	{
		if (!StaticMesh || !StaticMesh->StaticMeshAsset)
			return false;

		bool bSuccess = true;

		// StaticMesh의 모든 재질 순회
		for (const auto& MaterialPair : StaticMesh->StaticMeshAsset->Materials)
		{
			const std::string& MaterialName = MaterialPair.Key;
			const FObjMaterialInfo& Material = MaterialPair.Value;

			// 확산 텍스처가 있는 경우
			if (!Material.DiffuseTexture.empty())
			{
				// 텍스처 경로 구성
				// Contents/StaticMesh 폴더 내의 상대 경로로 변환
				std::string baseStr = "Contents/StaticMesh/" + Material.DiffuseTexture;
				fs::path texturePath = fs::path(baseStr);

				// 텍스처 이름으로 MaterialName 사용 (키값과 매핑)
				std::string textureName = MaterialName + "_Diffuse";

				// 텍스처 로드 및 SRV 생성
				try {
					std::shared_ptr<FTexture> TextureImage = FTexture::Load(texturePath.string(), textureName);
					if (TextureImage) {
						TextureImage->CreateShaderResourceView();
						// 성공적으로 로드된 텍스처 정보 출력 (디버깅용)
						std::cout << "Loaded texture: " << textureName << " from " << texturePath.string() << std::endl;
					}
					else {
						std::cerr << "Failed to load texture: " << texturePath.string() << std::endl;
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
	bool HasMesh(const std::string& MeshName)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);
		return MeshMap.Find(MeshName);
	}

	// 메시 제거
	void RemoveMesh(const std::string& MeshName)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);
		MeshMap.Remove(MeshName);
	}

	// 모든 메시 제거
	void ClearAllMeshes()
	{
		std::lock_guard<std::mutex> lock(MeshMutex);
		MeshMap.Empty();
	}

	// 로드된 모든 메시 이름 가져오기
	std::vector<std::string> GetAllMeshNames()
	{
		std::lock_guard<std::mutex> lock(MeshMutex);
		std::vector<std::string> names;
		names.reserve(MeshMap.Num ());

		for (const auto& pair : MeshMap)
		{
			names.push_back(pair.Key);
		}

		return names;
	}

	// 로드된 메시 수 가져오기
	size_t GetMeshCount()
	{
		std::lock_guard<std::mutex> lock(MeshMutex);
		return MeshMap.Num();
	}
private:
	// 메시 저장소
	TMap<std::string, std::shared_ptr<FStaticMesh>> MeshMap;
	std::mutex MeshMutex;
};
