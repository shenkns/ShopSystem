// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

#include "Module/ShopSystemModule.h"

#if UE_EDITOR
#include "ISettingsModule.h"
#include "Module/ShopSystemSettings.h"
#endif

IMPLEMENT_MODULE(FShopSystemModule, ShopSystem);

SHOPSYSTEM_API DEFINE_LOG_CATEGORY(LogShopSystem);

void FShopSystemModule::StartupModule()
{
#if UE_EDITOR
	RegisterSystemSettings();
#endif
}

void FShopSystemModule::ShutdownModule()
{
#if UE_EDITOR
	UnregisterSystemSettings();
#endif
}

#if UE_EDITOR
void FShopSystemModule::RegisterSystemSettings() const
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings(
			"Project",
			"Plugins",
			"Shop System",
			FText::FromString(TEXT("Shop System")),
			FText::FromString(TEXT("Configuration settings for Shop System")),
			GetMutableDefault<UShopSystemSettings>()
		);
	}
}

void FShopSystemModule::UnregisterSystemSettings() const
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "Shop System");
	}
}
#endif
