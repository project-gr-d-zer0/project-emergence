using System.IO;
using UnrealBuildTool;

public class EmergeCore : ModuleRules
{
    public EmergeCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "EnhancedInput", "NavigationSystem", "AIModule", "GameplayTasks" });
        PrivateDependencyModuleNames.AddRange(new string[] { });

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
            Path.Combine(ModuleDirectory, "Public", "Shell"), // Added for Shell area
        });

        string ProjectRootDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "..", ".."));
        string GeneratedDir = Path.Combine(ProjectRootDir, "Intermediate", "EmergeCoreGenerated");
        Directory.CreateDirectory(GeneratedDir);
        File.WriteAllText(Path.Combine(GeneratedDir, "CombatBallistics.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeCombatBallistics.h\"\n" +
            "using UCombatBallistics = UEmergeCombatBallistics;\n");

        File.WriteAllText(Path.Combine(GeneratedDir, "CraftingRecipe.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Crafting/EmergeCraftingRecipe.h\"\n" +
            "using UCraftingRecipe = UEmergeCraftingRecipe;\n");

        File.WriteAllText(Path.Combine(GeneratedDir, "BallisticAccuracyResolver.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeBallisticAccuracyResolver.h\"\n" +
            "using UBallisticAccuracyResolver = UEmergeBallisticAccuracyResolver;\n");

        string GeneratedWorldDir = Path.Combine(GeneratedDir, "World");
        Directory.CreateDirectory(GeneratedWorldDir);
        File.WriteAllText(Path.Combine(GeneratedWorldDir, "SafeZoneArbiter.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"World/EmergeSafeZoneArbiter.h\"\n" +
            "using USafeZoneArbiter = UEmergeSafeZoneArbiter;\n");

        File.WriteAllText(Path.Combine(GeneratedWorldDir, "WorldDepthDifficulty.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"World/EmergeWorldDepthDifficulty.h\"\n" +
            "using UWorldDepthDifficulty = UEmergeWorldDepthDifficulty;\n");

        string GeneratedCombatDir = Path.Combine(GeneratedDir, "Combat");
        Directory.CreateDirectory(GeneratedCombatDir);
        File.WriteAllText(Path.Combine(GeneratedCombatDir, "ArmorPenetrationResolver.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeArmorPenetrationResolver.h\"\n" +
            "using UArmorPenetrationResolver = UEmergeArmorPenetrationResolver;\n");
        File.WriteAllText(Path.Combine(GeneratedCombatDir, "ProjectileDefinition.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeProjectileDefinition.h\"\n" +
            "using UProjectileDefinition = UEmergeProjectileDefinition;\n");
        File.WriteAllText(Path.Combine(GeneratedCombatDir, "ArmorItem.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeArmorItem.h\"\n" +
            "using UArmorItem = UEmergeArmorItem;\n");

        File.WriteAllText(Path.Combine(GeneratedDir, "CombatFactionTier.h"),
            "#pragma once\n" +
            "// AUTO-GENERATED forwarding shim (see EmergeCore.Build.cs) — do not edit, do not check in.\n" +
            "#include \"Combat/EmergeFactionTier.h\"\n" +
            "using UCombatFactionTier = UEmergeFactionTier;\n");

        PublicIncludePaths.Add(GeneratedDir);
        PublicIncludePaths.Add(GeneratedWorldDir);
        PublicIncludePaths.Add(GeneratedCombatDir);
    }
}