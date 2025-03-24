#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "Core/HAL/PlatformType.h"
#include "Core/Container/Array.h"
#include "Archive.h"

// 파일 쓰기를 위한 클래스
class FWindowsBinWriter : public FArchive
{
public:
	FWindowsBinWriter(const std::string& FilePath)
	{
		FileStream.open(FilePath, std::ios::binary | std::ios::out);
		if (FileStream.is_open())
		{
			SetIsSaving(true);
		}
		else
		{
			SetError();
		}
	}

	virtual ~FWindowsBinWriter()
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
			FileStream.write(static_cast<const char*>(Data), Length);
			if (FileStream.fail())
			{
				SetError();
			}
		}
	}

	bool IsValid() const { return FileStream.is_open() && !IsError(); }

private:
	std::ofstream FileStream;
};