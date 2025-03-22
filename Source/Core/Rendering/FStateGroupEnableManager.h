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
	bool enableState = false;
	TMap < FName, bool> GroupStateMap;
};