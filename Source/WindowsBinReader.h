#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Array.h"
#include "Archive.h"

class FWindowsBinReader : public FArchive
{
public:
	FWindowsBinReader(const std::string& FilePath)
	{
		FileStream.open(FilePath, std::ios::binary | std::ios::in);
		if (FileStream.is_open())
		{
			SetIsLoading(true);
		}
		else
		{
			SetError();
		}
	}

	virtual ~FWindowsBinReader()
	{
		if (FileStream.is_open())
		{
			FileStream.close();
		}
	}

	virtual void Serialize(void* Data, uint64 Length) override
	{
		if (!IsError() && Length > 0)
		{
			FileStream.read(static_cast<char*>(Data), Length);
			if (FileStream.fail() && !FileStream.eof())
			{
				SetError();
			}
		}
	}

	bool IsValid() const { return FileStream.is_open() && !IsError(); }

private:
	std::ifstream FileStream;
};

// OBJ 모델 데이터를 바이너리로 저장하고 로드하는 유틸리티 클래스
class FObjBinaryManager
{
public:
	FObjBinaryManager() = default;
	~FObjBinaryManager() = default;

	// OBJ 파일을 로드하되, 바이너리 파일이 있으면 바이너리에서 로드
	//bool LoadMesh(const std::string& objFilePath, const std::string& binFilePath, FStaticMesh& outMesh)
	//{
	//	// 바이너리 파일이 존재하는지 확인
	//	if (DoesBinaryFileExist(binFilePath))
	//	{
	//		// 바이너리 파일에서 로드
	//		return LoadFromBinary(binFilePath, outMesh);
	//	}
	//	else
	//	{
	//		// OBJ 파일 파싱 후 바이너리로 저장
	//		if (ParseAndSaveToBinary(objFilePath, binFilePath))
	//		{
	//			return LoadFromBinary(binFilePath, outMesh);
	//		}
	//		return false;
	//	}
	//}
};