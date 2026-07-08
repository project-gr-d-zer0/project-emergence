using UnrealBuildTool;
public class EmergeCore : ModuleRules
{
    public EmergeCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // Public: exposed to dependent modules. Private: cpp-only (kept minimal to reduce compile time).
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameplayTags" });
        PrivateDependencyModuleNames.AddRange(new string[] { });
    }
}
