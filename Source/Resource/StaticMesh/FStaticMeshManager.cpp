#include "FStaticMeshManager.h"

std::shared_ptr<FStaticMesh> FStaticMeshManager::LoadObjStaticMeshAsset(const std::string& FilePath)
{
	std::lock_guard<std::mutex> lock(MeshMutex);

	// 이미 로드된 메시인지 확인
	if (MeshMap.Find(FilePath))
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
	}

	// 바이너리 파일 경로 생성
	std::string BinFilePath = "Contents/StaticMesh/" + BaseName + ".bin";

	// 없으면 새 메시 생성
	std::shared_ptr<FStaticMesh> NewMesh = std::make_shared<FStaticMesh>();

	// 바이너리 파일이 존재하는지 확인
	if (DoesBinaryFileExist(BinFilePath))
	{
		// 바이너리 파일에서 로드
		MeshMap[FilePath] = NewMesh;
		return LoadFromBinary(BinFilePath, NewMesh);
	}
	else
	{
		// OBJ 파일 파싱 후 바이너리로 저장
		if (ParseAndSaveToBinary(FilePath, BinFilePath))
		{
			MeshMap[FilePath] = NewMesh;
			return LoadFromBinary(BinFilePath, NewMesh);
		}
		return nullptr;
	}
}

UStaticMesh* FStaticMeshManager::LoadObjStaticMesh(const std::string& PathFileName)
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

std::shared_ptr<FStaticMesh> FStaticMeshManager::GetMesh(const std::string& MeshName)
{
	std::lock_guard<std::mutex> lock(MeshMutex);

	auto res = MeshMap.Find(MeshName);
	if (res)
	{
		return *res;
	}

	return nullptr; // 메시 없음
}

UStaticMesh* FStaticMeshManager::GetStaticMesh(const std::string& PathFileName)
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

bool FStaticMeshManager::LoadTexturesFromStaticMesh(UStaticMesh* StaticMesh)
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

bool FStaticMeshManager::HasMesh(const std::string& MeshName)
{
	std::lock_guard<std::mutex> lock(MeshMutex);
	return MeshMap.Find(MeshName);
}

void FStaticMeshManager::RemoveMesh(const std::string& MeshName)
{
	std::lock_guard<std::mutex> lock(MeshMutex);
	MeshMap.Remove(MeshName);
}

void FStaticMeshManager::ClearAllMeshes()
{
	std::lock_guard<std::mutex> lock(MeshMutex);
	MeshMap.Empty();
}

std::vector<std::string> FStaticMeshManager::GetAllMeshNames()
{
	std::lock_guard<std::mutex> lock(MeshMutex);
	std::vector<std::string> names;
	names.reserve(MeshMap.Num());

	for (const auto& pair : MeshMap)
	{
		names.push_back(pair.Key);
	}

	return names;
}

size_t FStaticMeshManager::GetMeshCount()
{
	std::lock_guard<std::mutex> lock(MeshMutex);
	return MeshMap.Num();
}

bool FStaticMeshManager::ParseAndSaveToBinary(const std::string& objFilePath, const std::string& binFilePath)
{
	// OBJ 파일 파싱
	FObjImporter importer;
	FStaticMesh mesh;

	if (!importer.ImportObjFile(objFilePath, mesh))
	{
		return false;
	}

	// 바이너리 파일로 저장
	return SaveStaticMeshToBinary(mesh, binFilePath);
}

bool FStaticMeshManager::SaveStaticMeshToBinary(const FStaticMesh& mesh, const std::string& binFilePath)
{
	FWindowsBinWriter writer(binFilePath);
	if (!writer.IsValid())
	{
		return false;
	}
	writer << mesh;

	return !writer.IsError();
}

std::shared_ptr<FStaticMesh> FStaticMeshManager::LoadFromBinary(const std::string& binFilePath, std::shared_ptr<FStaticMesh> OutMesh)
{
	FWindowsBinReader reader(binFilePath);
	if (!reader.IsValid())
	{
		return nullptr;
	}

	reader << *OutMesh;

	return OutMesh;
}

bool FStaticMeshManager::DoesBinaryFileExist(const std::string& binFilePath)
{
	std::ifstream file(binFilePath);
	return file.good();
}
