using System.IO;
using UnrealBuildTool;
public class EmergeCore : ModuleRules
{
    public EmergeCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // Public: exposed to dependent modules. Private: cpp-only (kept minimal to reduce compile time).
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "ALS" });
        PrivateDependencyModuleNames.AddRange(new string[] { });

        // Expose per-area public subdirs so headers can be included by leaf name
        // (e.g. #include "EmergeItemDefinition.h") as the acceptance tests do.
        PublicIncludePaths.AddRange(new string[]
        {
            Path.Combine(ModuleDirectory, "Public", "Items"),
            Path.Combine(ModuleDirectory, "Public", "Inventory"),
            Path.Combine(ModuleDirectory, "Public", "World"),
            Path.Combine(ModuleDirectory, "Public", "Combat"),
        });
    }
}
