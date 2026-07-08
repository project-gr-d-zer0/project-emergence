using UnrealBuildTool;
public class ProjectEmergenceTarget : TargetRules
{
    public ProjectEmergenceTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        bOverrideBuildEnvironment = true;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "ProjectEmergence", "EmergeCore" });
    }
}
