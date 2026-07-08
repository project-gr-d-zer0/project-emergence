using UnrealBuildTool;
public class ProjectEmergenceEditorTarget : TargetRules
{
    public ProjectEmergenceEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
        ExtraModuleNames.AddRange(new string[] { "ProjectEmergence", "EmergeCore" });
    }
}
