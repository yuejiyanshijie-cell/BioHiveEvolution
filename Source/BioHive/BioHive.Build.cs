// BioHive.Build.cs - 模块编译配置
// 生物母巢进化游戏核心模块

using UnrealBuildTool;

public class BioHive : ModuleRules
{
    public BioHive(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",
            "UMG",
            "Slate",
            "SlateCore",
            "Niagara",
            "AIModule",
            "MassEntity",
            "MassAI"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "GameplayAbilities",
            "MassCommon",
            "MassMovement"
        });

        // 优化编译
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }
    }
}
