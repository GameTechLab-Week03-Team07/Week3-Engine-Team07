#include "ConfigManager.h"
#include <filesystem>
#include <fstream>
#include <iostream>


using namespace std;

bool UConfigManager::LoadConfig(const FString& InConfigName)
{
	// if (IsDebuggerPresent())
	{
		filesystem::path curPath = filesystem::current_path();
		filesystem::path configPath = curPath / "Config" / InConfigName.GetData();
		if (filesystem::exists(configPath) == false)
		{
			std::cout << "Config file not found!" << std::endl;
			return false;
		}

		ifstream configFile(configPath);
		if (configFile.is_open() == false)
		{
			std::cout << "File open failed: " << configPath << std::endl;
			return false;
		}

		ostringstream oss;

		string line;
		string currentSection;

		while (getline(configFile, line))
		{
			line = trim(line);
			if (line.empty())
				continue; // 빈 줄 무시

			// 주석 처리된 줄 무시 (세미콜론(;) 또는 해시(#) 사용)
			if (line[0] == ';' || line[0] == '#')
				continue;

			// 섹션 처리: [SectionName]
			if (line.front() == '[' && line.back() == ']') {
				currentSection = trim(line.substr(1, line.size() - 2));
				continue;
			}

			// key = value 형태의 줄 처리
			auto pos = line.find('=');
			if (pos == string::npos)
				continue; // '='가 없으면 무시

			string key = trim(line.substr(0, pos));
			string value = trim(line.substr(pos + 1));
			Configs[currentSection][key] = value;
		}

		configFile.close();
		return true;
	}
	// else
	// {
	// 	// 빌드된 실행 파일의 경로로 설정
	// 	return false;
	// }

}

bool UConfigManager::SaveConfig(const FString& InConfigName)
{
	if (IsDebuggerPresent())
	{
		filesystem::path curPath = filesystem::current_path();
		filesystem::path configPath = curPath / "Config" / InConfigName.GetData();
		filesystem::path parentPath = configPath.parent_path();
		if (parentPath.empty() == false && filesystem::exists(parentPath) == false)
		{
			if (filesystem::create_directories(parentPath) == false)
			{
				std::cout << "Failed to create directory: " << parentPath << std::endl;
				return false;
			}
		}

		ofstream configFile(configPath);
		if (configFile.is_open() == false)
		{
			std::cout << "File open failed: " << configPath << std::endl;
			return false;
		}

		for (const auto& [section, keyValues] : Configs)
		{
			if (section.IsEmpty() == false)
				configFile << "[" << section.GetData() << "]" << std::endl;
			for (const auto& [key, value] : keyValues)
			{
				configFile << key.GetData() << " = " << value.GetData() << std::endl;
			}
			configFile << std::endl;
		}

		return true;
	}
	else
	{
		// 빌드된 실행 파일의 경로로 설정
		filesystem::path curPath = filesystem::current_path();
		filesystem::path configPath = curPath / "Config" / InConfigName.GetData();
		filesystem::path parentPath = configPath.parent_path();
		if (parentPath.empty() == false && filesystem::exists(parentPath) == false)
		{
			if (filesystem::create_directories(parentPath) == false)
			{
				std::cout << "Failed to create directory: " << parentPath << std::endl;
				return false;
			}
		}

		ofstream configFile(configPath);
		if (configFile.is_open() == false)
		{
			std::cout << "File open failed: " << configPath << std::endl;
			return false;
		}

		for (const auto& [section, keyValues] : Configs)
		{
			if (section.IsEmpty() == false)
				configFile << "[" << section.GetData() << "]" << std::endl;
			for (const auto& [key, value] : keyValues)
			{
				configFile << key.GetData() << " = " << value.GetData() << std::endl;
			}
			configFile << std::endl;
		}

		return true;
	}
}

FString UConfigManager::GetValue(const FString& InSection, const FString& InKey)
{
	auto data = Configs.Find(InSection);
	if (data != nullptr)
	{
		auto value = data->Find(InKey);
		if (value)
		{
			return *value;
		}
	}

	return "";
}

void UConfigManager::SetValue(const FString& InSection, const FString& InKey, const FString& InValue)
{
	Configs[InSection][InKey] = InValue;
}

string UConfigManager::trim(const string& InStr) const
{
	auto start = InStr.begin();
	// 앞쪽의 공백 제거
	while (start != InStr.end() && std::isspace(static_cast<unsigned char>(*start))) {
		++start;
	}

	// 문자열 전체가 공백이거나 빈 문자열인 경우, 빈 문자열 반환
	if (start == InStr.end())
		return "";

	// 마지막 유효 문자를 가리키도록 end 초기화
	auto end = InStr.end() - 1;
	// 뒤쪽의 공백 제거
	while (end != start && std::isspace(static_cast<unsigned char>(*end))) {
		--end;
	}

	// [start, end] 구간의 문자열 반환 (end+1은 end의 다음 iterator)
	return string(start, end + 1);
}
