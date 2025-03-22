#pragma once
#include "Core/UObject/Object.h"
#include "StaticMeshTypes.h"

class UStaticMesh : public UObject
{
	DECLARE_CLASS(UStaticMesh, UObject)

public:
	UStaticMesh() = default;

	std::shared_ptr<FStaticMesh> StaticMeshAsset;

	const std::string& GetAssetPathFileName()
	{
		return StaticMeshAsset->PathFileName;
	}

	void SetStaticMeshAsset(std::shared_ptr<FStaticMesh> InStaticMesh)
	{
		StaticMeshAsset = InStaticMesh;
	}
};