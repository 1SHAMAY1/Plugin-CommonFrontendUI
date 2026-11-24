using UnrealBuildTool;

public class CommonFrontendUI_LoadingScreen : ModuleRules
{
    public CommonFrontendUI_LoadingScreen(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "Engine",
                "PreLoadScreen",
                "UMG",
                "DeveloperSettings",
                
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Slate",
                "SlateCore",
                "RenderCore",      
                "RHI",
            }
            
        );
        
    }
}