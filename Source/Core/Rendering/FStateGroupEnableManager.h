#pragma once
#include <Core/UObject/NameTypes.h>
#include <Core/Container/Map.h>
#include "Core/AbstractClass/Singleton.h"
class FStateGroupEnableManager : public TSingleton<FStateGroupEnableManager> {
public:
	FStateGroupEnableManager();
	bool IsGroupEnabled(FName GroupName);
	void EnableGroup(FName GroupName);
	void DisableGroup(FName GroupName);
	void Initialize();
	void EnableStat();
	void DisableStat();
	bool IsEnableStat();

private:
	// 스탯 활성화 상태
	bool enableState = false;
	// 스탯 그룹 활성화 상태
	TMap < FName, bool> GroupStateMap;
};