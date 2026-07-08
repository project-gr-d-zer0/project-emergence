using UnrealBuildTool;
// Dedicated Linux server target (spec 10.1). NOTE: compiling this REQUIRES a source (GitHub) UE5.8 build.
public class ProjectEmergenceServerTarget : TargetRules
{
    public ProjectEmergenceServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "ProjectEmergence", "EmergeCore" });
    }
}
