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
#include "WindowsBinWriter.h"
#include "WindowsBinReader.h"

// 메시 매니저 클래스 (일단 싱글톤으로 작성함)
class FStaticMeshManager : public TSingleton<FStaticMeshManager>
{
public:
	// 메시 로드 및 저장
	std::shared_ptr<FStaticMesh> LoadObjStaticMeshAsset(const std::string& FilePath)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);

		// 이미 로드된 메시인지 확인
		/*if (MeshMap.Find(FilePath))
		{
			return *MeshMap.Find(FilePath);
		}

		std::string BaseName = FilePath;
		size_t lastDot = BaseName.find_last_of('.');
		if (lastDot != std::string::npos)
		{
			BaseName = BaseName.substr(0, lastDot);
		}

		size_t lastSlash = BaseName.find_last_of("/\\");
		if (lastSlash != std::string::npos)
		{
			BaseName = BaseName.substr(lastSlash + 1);
		}*/

		// 바이너리 파일 경로 생성
		//std::string BinFilePath = "Contents/StaticMesh/" + BaseName + ".bin";

		// 없으면 새 메시 생성
		std::shared_ptr<FStaticMesh> NewMesh = std::make_shared<FStaticMesh>();

		// 바이너리 파일이 존재하는지 확인
		//if (DoesBinaryFileExist(BinFilePath))
		//{
		//	// 바이너리 파일에서 로드
		//	// TEMP
		//	MeshMap[FilePath] = NewMesh;
		//	return LoadFromBinary(BinFilePath, NewMesh);
		//}
		//else
		//{
		//	// OBJ 파일 파싱 후 바이너리로 저장
		//	if (ParseAndSaveToBinary(FilePath, BinFilePath))
		//	{
		//		return LoadFromBinary(BinFilePath, NewMesh);
		//	}
		//	return nullptr;
		//}

		FObjImporter importer;
		if (!importer.ImportObjFile(FilePath, *NewMesh))
		{
			return nullptr; // 로드 실패
		}

		// 메시 맵에 저장
		MeshMap[FilePath] = NewMesh;
		return NewMesh;
	}
	// UStaticMesh를 로드하는 함수 (UObject를 상속받아 UUID가 생긴 상태)
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

	// 메시 정보 가져오기
	std::shared_ptr<FStaticMesh> GetMesh(const std::string& MeshName)
	{
		std::lock_guard<std::mutex> lock(MeshMutex);

		auto res = MeshMap.Find(MeshName);
		if (res)
		{
			return *res;
		}

		return nullptr; // 메시 없음
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

	// UStaticMesh의 텍스처 정보를 활용하여 SRV 생성하는 함수
	bool LoadTexturesFromStaticMesh(UStaticMesh* StaticMesh)
	{
		if (!StaticMesh || !StaticMesh->StaticMeshAsset)
			return false;

		bool bSuccess = true;

		// StaticMesh의 모든 재질 순회 - 추후 확산 텍스처 맵 이외 다른 처리도 필요할 듯
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

	// OBJ 파일을 파싱하고 바이너리로 저장
	bool ParseAndSaveToBinary(const std::string& objFilePath, const std::string& binFilePath)
	{
		// OBJ 파일 파싱
		FObjImporter importer;
		FObjInfo objInfo;
		FStaticMesh mesh;

		if (!importer.ImportObjFile(objFilePath, mesh))
		{
			return false;
		}

		// 바이너리 파일로 저장
		return SaveStaticMeshToBinary(mesh, binFilePath);
	}

	// 메시 데이터를 바이너리 파일로 저장
	bool SaveStaticMeshToBinary(const FStaticMesh& mesh, const std::string& binFilePath)
	{
		FWindowsBinWriter writer(binFilePath);
		if (!writer.IsValid())
		{
			return false;
		}

		// 복사본 생성 후 저장
		std::string pathName = mesh.PathFileName;
		TArray<FVertexSimple> verticesCopy = mesh.Vertices;
		TArray<uint32> indicesCopy = mesh.Indices;
		writer << pathName;
		writer << verticesCopy;
		writer << indicesCopy;
		// 정점 데이터 저장
		//writer << mesh.Vertices;

		// 인덱스 데이터 저장
		//writer << mesh.Indices;

		//// 머티리얼 정보 저장
		//int32 materialCount = static_cast<int32>(mesh.Materials.Num());
		//writer << materialCount;

		//for (const auto& materialPair : mesh.Materials)
		//{
		//	std::string materialName = materialPair.Key;
		//	writer << materialName;

		//	const FObjMaterialInfo& material = materialPair.Value;

		//	/*writer << material.DiffuseColor.X << material.DiffuseColor.Y << material.DiffuseColor.Z;
		//	writer << material.AmbientColor.X << material.AmbientColor.Y << material.AmbientColor.Z;
		//	writer << material.SpecularColor.X << material.SpecularColor.Y << material.SpecularColor.Z;
		//	writer << material.SpecularExponent;
		//	writer << material.Opacity;
		//	writer << material.DiffuseTexture;
		//	writer << material.PathFileName;*/
		//}

		return !writer.IsError();
	}

	// 바이너리 파일에서 메시 데이터 로드
	std::shared_ptr<FStaticMesh> LoadFromBinary(const std::string& binFilePath, std::shared_ptr<FStaticMesh> OutMesh)
	{
		FWindowsBinReader reader(binFilePath);
		if (!reader.IsValid())
		{
			return nullptr;
		}
		// Name 정보 읽기
		reader << OutMesh->PathFileName;

		// 정점 데이터 읽기
		reader << OutMesh->Vertices;

		// 인덱스 데이터 읽기
		reader << OutMesh->Indices;

		// 머티리얼 정보 읽기
		int32 materialCount = 0;
		reader << materialCount;

		for (int32 i = 0; i < materialCount; i++)
		{
			std::string materialName;
			reader << materialName;

			FObjMaterialInfo material;
			reader << material.DiffuseColor.X << material.DiffuseColor.Y << material.DiffuseColor.Z;
			reader << material.AmbientColor.X << material.AmbientColor.Y << material.AmbientColor.Z;
			reader << material.SpecularColor.X << material.SpecularColor.Y << material.SpecularColor.Z;
			reader << material.SpecularExponent;
			reader << material.Opacity;
			reader << material.DiffuseTexture;
			reader << material.PathFileName;

			OutMesh->Materials[materialName] = material;
		}

		return OutMesh;
	}

	// 바이너리 파일이 존재하는지 확인
	bool DoesBinaryFileExist(const std::string& binFilePath)
	{
		std::ifstream file(binFilePath);
		return file.good();
	}

private:
	// 메시 저장소
	TMap<std::string, std::shared_ptr<FStaticMesh>> MeshMap;
	std::mutex MeshMutex;
};
