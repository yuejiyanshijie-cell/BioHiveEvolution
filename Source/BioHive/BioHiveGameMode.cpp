// BioHiveGameMode.cpp - 游戏模式实现

#include "BioHiveGameMode.h"
#include "BioUnit.h"
#include "BioBuilding.h"
#include "Systems/BioResourceManager.h"
#include "Units/BioUnitSelectionManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABioHiveGameMode::ABioHiveGameMode()
{
    // 设置默认Pawn类
    DefaultPawnClass = nullptr; // 使用相机代替Pawn

    // 默认参数
    HiveLevel = 1;
    MaxHiveLevel = 5;

    // 每级升级消耗
    HiveUpgradeCosts = { 0.0f, 500.0f, 1000.0f, 2000.0f, 5000.0f, 10000.0f };

    // 每级人口上限
    MaxPopulationPerLevel = { 0, 20, 40, 60, 100, 200 };
}

void ABioHiveGameMode::InitGame(const FString& MapName, const FString& Options,
    FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void ABioHiveGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 创建资源管理器
    ResourceManager = GetWorld()->SpawnActor<ABioResourceManager>();
    if (ResourceManager)
    {
        // 设置初始资源
        ResourceManager->AddBiomass(200.0f);
        ResourceManager->AddGeneFragments(5);
    }

    // 创建选择管理器
    SelectionManager = GetWorld()->SpawnActor<ABioUnitSelectionManager>();

    // 创建初始母巢核心
    if (HiveCoreClass)
    {
        FVector SpawnLocation = FVector::ZeroVector;
        // 查找玩家出生点
        AActor* PlayerStart = UGameplayStatics::GetActorOfClass(
            GetWorld(), APlayerStart::StaticClass());
        if (PlayerStart)
        {
            SpawnLocation = PlayerStart->GetActorLocation();
        }

        ABioBuilding* HiveCore = GetWorld()->SpawnActor<ABioBuilding>(
            HiveCoreClass, SpawnLocation, FRotator::ZeroRotator);
        if (HiveCore)
        {
            HiveCore->DisablePlacementMode(); // 直接开始建造
        }
    }
}

bool ABioHiveGameMode::UpgradeHive()
{
    if (HiveLevel >= MaxHiveLevel) return false;
    if (!ResourceManager) return false;

    float Cost = GetHiveUpgradeCost();
    if (!ResourceManager->SpendBiomass(Cost)) return false;

    HiveLevel++;
    OnHiveLevelUp(HiveLevel);
    return true;
}

float ABioHiveGameMode::GetHiveUpgradeCost() const
{
    if (HiveLevel < HiveUpgradeCosts.Num())
    {
        return HiveUpgradeCosts[HiveLevel];
    }
    return 0.0f;
}

int32 ABioHiveGameMode::GetMaxUnitCount() const
{
    if (HiveLevel < MaxPopulationPerLevel.Num())
    {
        return MaxPopulationPerLevel[HiveLevel];
    }
    return 20;
}

int32 ABioHiveGameMode::GetCurrentUnitCount() const
{
    RegisteredUnits.RemoveAll([](const TWeakObjectPtr<ABioUnit>& U) {
        return !U.IsValid();
    });
    return RegisteredUnits.Num();
}

int32 ABioHiveGameMode::GetMaxPopulation() const
{
    return MaxPopulationPerLevel[FMath::Clamp(HiveLevel, 0, MaxHiveLevel)];
}

float ABioHiveGameMode::GetPopulationPercent() const
{
    int32 MaxPop = GetMaxPopulation();
    if (MaxPop <= 0) return 1.0f;
    return static_cast<float>(GetCurrentUnitCount()) / static_cast<float>(MaxPop);
}

ABioUnit* ABioHiveGameMode::SpawnUnit(TSubclassOf<ABioUnit> UnitClass, FVector Location)
{
    if (!UnitClass) return nullptr;
    if (GetCurrentUnitCount() >= GetMaxPopulation()) return nullptr;

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    ABioUnit* NewUnit = GetWorld()->SpawnActor<ABioUnit>(
        UnitClass, Location, FRotator::ZeroRotator, Params);

    if (NewUnit)
    {
        RegisterUnit(NewUnit);
    }

    return NewUnit;
}

void ABioHiveGameMode::RegisterUnit(ABioUnit* Unit)
{
    if (Unit)
    {
        RegisteredUnits.Add(Unit);
    }
}

void ABioHiveGameMode::UnregisterUnit(ABioUnit* Unit)
{
    RegisteredUnits.RemoveAll([Unit](const TWeakObjectPtr<ABioUnit>& U) {
        return U.Get() == Unit;
    });
}

bool ABioHiveGameMode::ResearchTechnology(FName TechnologyName)
{
    if (IsTechnologyResearched(TechnologyName)) return false;

    // 消耗资源
    if (!ResourceManager || !ResourceManager->SpendGeneFragments(5)) return false;

    ResearchedTechnologies.Add(TechnologyName);
    return true;
}

bool ABioHiveGameMode::IsTechnologyResearched(FName TechnologyName) const
{
    return ResearchedTechnologies.Contains(TechnologyName);
}
