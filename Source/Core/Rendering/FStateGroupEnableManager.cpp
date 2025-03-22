#include "FStateGroupEnableManager.h"

FStateGroupEnableManager::FStateGroupEnableManager()
{
	Initialize();
}

// 스탯 활성화 여부 반환.
bool FStateGroupEnableManager::IsGroupEnabled(FName GroupName)
{
	if (GroupStateMap[GroupName] == true) {
		return true;
	}
	else {
		return false;
	}
}

// 해당 그룹 활성화
void FStateGroupEnableManager::EnableGroup(FName GroupName)
{
	GroupStateMap[GroupName] = true;
}

// 그룹 비활성화
void FStateGroupEnableManager::DisableGroup(FName GroupName)
{
	GroupStateMap[GroupName] = false;
	
}

// 그룹 초기화
void FStateGroupEnableManager::Initialize() {
	GroupStateMap["FPS"] = false;
	GroupStateMap["Memory"] = false;
}

// 스탯 창 활성화
void FStateGroupEnableManager::EnableStat()
{
	enableState = true;
}

// 스탯 창 비활성화
void FStateGroupEnableManager::DisableStat() {
	enableState = false;
	for (auto group : GroupStateMap) {
		group.Value = false;
	}
}

// 스탯 창 활성화 여부
bool FStateGroupEnableManager::IsEnableStat()
{
	return enableState;
}