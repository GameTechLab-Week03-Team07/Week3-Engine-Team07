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

	for (auto& [CameraType, CamerasArray] : Json["Camera"].ObjectRange())
	{
		for (JSON& CameraInfo : CamerasArray.ArrayRange())
		{
			std::unique_ptr<ACameraInfo> Camera = std::make_unique<ACameraInfo>();
			std::string temp = CameraType;
			Camera->ProjectionMode = CameraType == "PerspectiveCamera" ? ECameraProjectionMode::Perspective : ECameraProjectionMode::Orthographic;
			Camera->OrthoViewMode = CameraInfo["OrthoViewMode"].ToString() == "Front" ? EOrthoViewMode::Front : CameraInfo["OrthoViewMode"].ToString() == "Side" ? EOrthoViewMode::Side : CameraInfo["OrthoViewMode"].ToString() == "Top" ? EOrthoViewMode::Top : EOrthoViewMode::None;
			JSON Location = CameraInfo["Location"];
			JSON Rotation = CameraInfo["Rotation"];
			Camera->Location = FVector(
				static_cast<float>(Location[0].ToFloat()),
				static_cast<float>(Location[1].ToFloat()),
				static_cast<float>(Location[2].ToFloat())
			);
			Camera->Rotation = FVector(
				static_cast<float>(Rotation[0].ToFloat()),
				static_cast<float>(Rotation[1].ToFloat()),
				static_cast<float>(Rotation[2].ToFloat())
			);
			Camera->Fov = CameraInfo["Fov"].ToFloat();
			Camera->NearClip = CameraInfo["NearClip"].ToFloat();
			Camera->FarClip = CameraInfo["FarClip"].ToFloat();
			WorldInfo->CameraInfos.push(std::move(Camera));
		}

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
	while (!WorldInfo.CameraInfos.empty())
	{
		const std::unique_ptr<ACameraInfo> CameraInfo = std::move(WorldInfo.CameraInfos.front());
		WorldInfo.CameraInfos.pop();

		std::string CameraType = CameraInfo->ProjectionMode == ECameraProjectionMode::Perspective ? "PerspectiveCamera" : "OrthographicCamera";
		std::string OrthoViewMode = CameraInfo->OrthoViewMode == EOrthoViewMode::Front ? "Front" : CameraInfo->OrthoViewMode == EOrthoViewMode::Side ? "Side" : CameraInfo->OrthoViewMode == EOrthoViewMode::Top ? "Top" : "None";
		
		Json["Camera"][CameraType].append(JSON::Make(JSON::Class::Object));

		int index = Json["Camera"][CameraType].size() - 1;
		Json["Camera"][CameraType][index]["Location"].append(CameraInfo->Location.X, CameraInfo->Location.Y, CameraInfo->Location.Z);
		Json["Camera"][CameraType][index]["Rotation"].append(CameraInfo->Rotation.X, CameraInfo->Rotation.Y, CameraInfo->Rotation.Z);
		Json["Camera"][CameraType][index]["Fov"] = CameraInfo->Fov;
		Json["Camera"][CameraType][index]["NearClip"] = CameraInfo->NearClip;
		Json["Camera"][CameraType][index]["FarClip"] = CameraInfo->FarClip;
		Json["Camera"][CameraType][index]["OrthoViewMode"] = OrthoViewMode;
	}
     
    std::ofstream Output(WorldInfo.SceneName + ".scene");
    
    if (Output.is_open())
    {
        Output << Json;
    }
}
