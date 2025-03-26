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