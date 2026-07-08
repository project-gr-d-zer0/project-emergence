using UnrealBuildTool;
public class ProjectEmergence : ModuleRules
{
    public ProjectEmergence(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "InputCore", "EmergeCore"
        });
        // GAS + tags for the ability/damage system (spec 11.2). More engine modules (MassEntity,
        // StateTree, Iris, Chaos, PoseSearch) get added here as their phase-0 systems land.
        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities", "GameplayTags", "GameplayTasks"
        });
    }
}
