// BioHiveGameMode.h - 生物母巢游戏模式
// 管理游戏核心循环：资源、建筑、单位

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BioHiveGameMode.generated.h"

class ABioResourceManager;
class ABioUnitSelectionManager;
class ABioRTSCamera;

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioHiveGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABioHiveGameMode();

    virtual void InitGame(const FString& MapName, const FString& Options,
        FString& ErrorMessage) override;

    virtual void BeginPlay() override;

    // --- 母巢等级 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Hive")
    int32 GetHiveLevel() const { return HiveLevel; }

    UFUNCTION(BlueprintCallable, Category = "Bio|Hive")
    bool UpgradeHive();

    UFUNCTION(BlueprintPure, Category = "Bio|Hive")
    float GetHiveUpgradeCost() const;

    UFUNCTION(BlueprintPure, Category = "Bio|Hive")
    int32 GetMaxUnitCount() const;

    // --- 人口系统 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Population")
    int32 GetCurrentUnitCount() const;

    UFUNCTION(BlueprintPure, Category = "Bio|Population")
    int32 GetMaxPopulation() const;

    UFUNCTION(BlueprintPure, Category = "Bio|Population")
    float GetPopulationPercent() const;

    // --- 单位管理 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Unit")
    ABioUnit* SpawnUnit(TSubclassOf<class ABioUnit> UnitClass, FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Bio|Unit")
    void RegisterUnit(ABioUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Bio|Unit")
    void UnregisterUnit(ABioUnit* Unit);

    // --- 管理器引用 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Manager")
    ABioResourceManager* GetResourceManager() const { return ResourceManager; }

    UFUNCTION(BlueprintPure, Category = "Bio|Manager")
    ABioUnitSelectionManager* GetSelectionManager() const { return SelectionManager; }

    // --- 科技系统 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Technology")
    bool ResearchTechnology(FName TechnologyName);

    UFUNCTION(BlueprintPure, Category = "Bio|Technology")
    bool IsTechnologyResearched(FName TechnologyName) const;

protected:
    // 蓝图可重写事件
    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Event")
    void OnHiveLevelUp(int32 NewLevel);

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Event")
    void OnGameOver(bool bWin);

    // 配置属性
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Hive")
    int32 HiveLevel = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Hive")
    int32 MaxHiveLevel = 5;

    // 每级升级消耗（生物质）
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Hive")
    TArray<float> HiveUpgradeCosts;

    // 每级人口上限
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Hive")
    TArray<int32> MaxPopulationPerLevel;

    // 默认单位类
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Unit")
    TSubclassOf<ABioUnit> DroneClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Unit")
    TSubclassOf<ABioUnit> WarriorClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Unit")
    TSubclassOf<ABioUnit> HydraliskClass;

    // 默认建筑类
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Building")
    TSubclassOf<class ABioBuilding> HiveCoreClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Building")
    TSubclassOf<ABioBuilding> HatcheryClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Building")
    TSubclassOf<ABioBuilding> DefenseTowerClass;

    // 已研究的科技
    UPROPERTY()
    TSet<FName> ResearchedTechnologies;

    // 管理器
    UPROPERTY()
    ABioResourceManager* ResourceManager;

    UPROPERTY()
    ABioUnitSelectionManager* SelectionManager;

    // 所有注册的单位
    UPROPERTY()
    TArray<TWeakObjectPtr<ABioUnit>> RegisteredUnits;

    // 胜利条件
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Victory")
    bool bEliminationVictory = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|Victory")
    int32 BossesDefeatedRequired = 3;
};
