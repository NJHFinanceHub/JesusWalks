using UnrealBuildTool;

public class TheNazareneAAA : ModuleRules
{
    public TheNazareneAAA(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "EnhancedInput",
                "UMG",
                "AIModule",
                "GameplayAbilities",
                "GameplayTags",
                "GameplayTasks",
                "Niagara"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Slate",
                "SlateCore"
            }
        );
    }
}

