using UnrealBuildTool;
using System.Collections.Generic;

public class TheNazareneAAATarget : TargetRules
{
    public TheNazareneAAATarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("TheNazareneAAA");
    }
}
