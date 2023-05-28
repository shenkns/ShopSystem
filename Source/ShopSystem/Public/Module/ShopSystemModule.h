// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#pragma once

#include "Modules/ModuleManager.h"

SHOPSYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogShopSystem, Log, All);

class FShopSystemModule : public IModuleInterface
{
public:

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:

#if UE_EDITOR
	void RegisterSystemSettings() const;
	void UnregisterSystemSettings() const;
#endif
};
