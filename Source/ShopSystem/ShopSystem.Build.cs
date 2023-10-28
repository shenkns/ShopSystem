// Copyright shenkns Shop System Developed With Unreal Engine. All Rights Reserved 2023.

using UnrealBuildTool;

public class ShopSystem : ModuleRules
{
	public ShopSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateIncludePaths.AddRange(
			new string[] {
				"ShopSystem/Public/"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"LogSystem",
				"ManagersSystem",
				"SaveLoadSystem",
				"DataSystem",
				"CurrenciesSystem",
				"SerializationSystem",
				"VaRest",
				"UMG",
				"WidgetsSystem"
			}
		);
		
		PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"LogSystem",
				"ManagersSystem",
				"SaveLoadSystem",
				"DataSystem",
				"CurrenciesSystem",
				"SerializationSystem",
				"VaRest",
				"WidgetsSystem"
			}
		);
	}
}
