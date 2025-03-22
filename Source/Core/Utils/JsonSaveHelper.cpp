#include "JsonSaveHelper.h"

#include <fstream>
#include <ranges>

#include "Core/EngineStatics.h"
#include "Debug/DebugConsole.h"
#include "SimpleJSON/Json.hpp"

using json::JSON;

// SceneName - 확장자 제외
std::unique_ptr<UWorldInfo> JsonSaveHelper::LoadScene(const std::string& SceneName)
{
	std::ifstream Input(SceneName + ".scene");

	if (!Input.is_open())
	{
		UE_LOG("Scene file not found");
		return nullptr;
	}
	std::string Contents;
	Input.seekg(0, std::ios::end);
	Contents.reserve(Input.tellg());
	Input.seekg(0, std::ios::beg);

	Contents.assign(std::istreambuf_iterator<char>(Input), std::istreambuf_iterator<char>());

	JSON Json = JSON::Load(Contents);

	std::unique_ptr<UWorldInfo> WorldInfo = std::make_unique<UWorldInfo>();
     
    WorldInfo->Version = Json["Version"].ToInt();
    WorldInfo->SceneName = Json["SceneName"].ToString();
    WorldInfo->ActorCount = Json["ActorCount"].ToInt();
	UEngineStatics::NextUUID = Json["NextUUID"].ToInt();
	for (JSON& ActorInfo : Json["Actors"].ObjectRange() | std::views::values)
    {
        std::unique_ptr<UObjectInfo> ObjectInfo = std::make_unique<UObjectInfo>();

        JSON Location = ActorInfo["Location"];
        JSON Rotation = ActorInfo["Rotation"];
        JSON Scale = ActorInfo["Scale"];
        ObjectInfo->Location = FVector(
			static_cast<float>(Location[0].ToFloat()), 
			static_cast<float>(Location[1].ToFloat()),
			static_cast<float>(Location[2].ToFloat())
		);
        ObjectInfo->Rotation = FVector(
			static_cast<float>(Rotation[0].ToFloat()), 
			static_cast<float>(Rotation[1].ToFloat()),
			static_cast<float>(Rotation[2].ToFloat())
		);
        ObjectInfo->Scale = FVector(
			static_cast<float>(Scale[0].ToFloat()), 
			static_cast<float>(Scale[1].ToFloat()),
			static_cast<float>(Scale[2].ToFloat())
		);

        ObjectInfo->ObjectType = ActorInfo["Type"].ToString();
		if (ObjectInfo->ObjectType == "StaticMesh" && ActorInfo.hasKey("StaticMeshAssetPath"))
		{
			ObjectInfo->StaticMeshAssetPath = ActorInfo["StaticMeshAssetPath"].ToString();
		}
		WorldInfo->ObjectInfos.push(std::move(ObjectInfo));
    }
    return WorldInfo;
}

void JsonSaveHelper::SaveScene(UWorldInfo WorldInfo)
{
	if (WorldInfo.SceneName.empty())
		return;
	JSON Json;

	Json["Version"] = WorldInfo.Version;
	Json["NextUUID"] = UEngineStatics::NextUUID;
	Json["ActorCount"] = WorldInfo.ActorCount;
	Json["SceneName"] = WorldInfo.SceneName;

    // for (uint32 i = 0; i < WorldInfo.ActorCount; i++)
	while (!WorldInfo.ObjectInfos.empty())
    {
        const std::unique_ptr<UObjectInfo> ObjectInfo = std::move(WorldInfo.ObjectInfos.front());
		WorldInfo.ObjectInfos.pop();

        std::string Uuid = std::to_string(ObjectInfo->UUID);
        
        Json["Actors"][Uuid]["Location"].append(ObjectInfo->Location.X, ObjectInfo->Location.Y, ObjectInfo->Location.Z);
        Json["Actors"][Uuid]["Rotation"].append(ObjectInfo->Rotation.X, ObjectInfo->Rotation.Y, ObjectInfo->Rotation.Z);
        Json["Actors"][Uuid]["Scale"].append(ObjectInfo->Scale.X, ObjectInfo->Scale.Y, ObjectInfo->Scale.Z);
        Json["Actors"][Uuid]["Type"] = ObjectInfo->ObjectType;

		if (ObjectInfo->ObjectType == "StaticMesh" and !ObjectInfo->StaticMeshAssetPath.empty()) 
		{
			Json["Actors"][Uuid]["StaticMeshAssetPath"] = ObjectInfo->StaticMeshAssetPath;
		}
    }
     
    std::ofstream Output(WorldInfo.SceneName + ".scene");
    
    if (Output.is_open())
    {
        Output << Json;
    }
}
