using UnrealBuildTool;

public class EmergeCoreALS : ModuleRules
{
    public EmergeCoreALS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "EmergeCore",
            "ALS", "ALSExtras", "ALSCamera",
            "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks" });
    }
}
