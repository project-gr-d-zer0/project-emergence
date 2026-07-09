using UnrealBuildTool;
public class ProjectEmergenceEditorTarget : TargetRules
{
    public ProjectEmergenceEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V7;
        bOverrideBuildEnvironment = true;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		// RAD linker (~50%% faster links) on build hosts only - mutually exclusive with Live Coding
		// (UE-230373), so gated behind a machine-level env var set on l3e7, never on the laptop.
		if (System.Environment.GetEnvironmentVariable("EMERGE_RAD_LINKER") == "1")
		{
			WindowsPlatform.bAllowRadLinker = true;
		}
        ExtraModuleNames.AddRange(new string[] { "ProjectEmergence", "EmergeCore" });
    }
}
