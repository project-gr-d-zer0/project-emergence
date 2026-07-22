using System.IO;
using UnrealBuildTool;

public class EmergeCore : ModuleRules
{
    public EmergeCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks" });
        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Short-name include roots. Compat/ holds committed forwarding shims (formerly Build.cs codegen
        // into Intermediate/ — removed 2026-07-22: unsupported, non-portable, broke cold/fresh builds).
        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public", "Items"),
            Path.Combine(ModuleDirectory, "Public", "Inventory"),
            Path.Combine(ModuleDirectory, "Public", "World"),
            Path.Combine(ModuleDirectory, "Public", "Combat"),
            Path.Combine(ModuleDirectory, "Public", "Survival"),
            Path.Combine(ModuleDirectory, "Public", "Nav"),
            Path.Combine(ModuleDirectory, "Public", "Anim"),
            Path.Combine(ModuleDirectory, "Public", "Crafting"),
            Path.Combine(ModuleDirectory, "Public", "Shell"),
            Path.Combine(ModuleDirectory, "Public", "Compat"),
        });
    }
}
