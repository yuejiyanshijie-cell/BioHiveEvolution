// BioResourceManager.cpp - 全局资源管理器实现

#include "BioResourceManager.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

ABioResourceManager::ABioResourceManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

ABioResourceManager* ABioResourceManager::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;
    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    // 从GameState获取单例
    ABioResourceManager* Manager = Cast<ABioResourceManager>(
        World->GetGameState());
    return Manager;
}

void ABioResourceManager::BeginPlay()
{
    Super::BeginPlay();
    // 广播初始值
    BroadcastResourceChange(EBioResourceType::Biomass);
    BroadcastResourceChange(EBioResourceType::GeneFragment);
    BroadcastResourceChange(EBioResourceType::Energy);
}

void ABioResourceManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 自动收入计算
    if (BiomassIncome > 0.0f)
    {
        AddBiomass(BiomassIncome * DeltaTime);
    }
    if (EnergyIncome > 0.0f)
    {
        Energy = FMath::Min(Energy + EnergyIncome * DeltaTime, MaxEnergy);
        BroadcastResourceChange(EBioResourceType::Energy);
    }
}

float ABioResourceManager::GetResource(EBioResourceType Type) const
{
    switch (Type)
    {
    case EBioResourceType::Biomass: return Biomass;
    case EBioResourceType::GeneFragment: return static_cast<float>(GeneFragments);
    case EBioResourceType::Energy: return Energy;
    default: return 0.0f;
    }
}

float ABioResourceManager::GetMaxStorage(EBioResourceType Type) const
{
    switch (Type)
    {
    case EBioResourceType::Biomass: return MaxBiomass;
    case EBioResourceType::GeneFragment: return static_cast<float>(MaxGeneFragments);
    case EBioResourceType::Energy: return MaxEnergy;
    default: return 0.0f;
    }
}

bool ABioResourceManager::SpendResources(const TArray<FBioResourceCost>& Costs)
{
    if (!CanAfford(Costs)) return false;

    for (const FBioResourceCost& Cost : Costs)
    {
        switch (Cost.ResourceType)
        {
        case EBioResourceType::Biomass:
            Biomass -= Cost.Amount;
            break;
        case EBioResourceType::GeneFragment:
            GeneFragments -= FMath::RoundToInt(Cost.Amount);
            break;
        case EBioResourceType::Energy:
            Energy -= Cost.Amount;
            break;
        default: break;
        }
        BroadcastResourceChange(Cost.ResourceType);
    }
    return true;
}

bool ABioResourceManager::SpendBiomass(float Amount)
{
    if (Biomass < Amount) return false;
    Biomass -= Amount;
    BroadcastResourceChange(EBioResourceType::Biomass);
    return true;
}

bool ABioResourceManager::SpendGeneFragments(int32 Amount)
{
    if (GeneFragments < Amount) return false;
    GeneFragments -= Amount;
    BroadcastResourceChange(EBioResourceType::GeneFragment);
    return true;
}

bool ABioResourceManager::SpendEnergy(float Amount)
{
    if (Energy < Amount) return false;
    Energy -= Amount;
    BroadcastResourceChange(EBioResourceType::Energy);
    return true;
}

bool ABioResourceManager::CanAfford(const TArray<FBioResourceCost>& Costs) const
{
    for (const FBioResourceCost& Cost : Costs)
    {
        float Current = GetResource(Cost.ResourceType);
        if (Current < Cost.Amount) return false;
    }
    return true;
}

void ABioResourceManager::AddResource(EBioResourceType Type, float Amount)
{
    switch (Type)
    {
    case EBioResourceType::Biomass: AddBiomass(Amount); break;
    case EBioResourceType::GeneFragment: AddGeneFragments(FMath::RoundToInt(Amount)); break;
    case EBioResourceType::Energy:
        Energy = FMath::Min(Energy + Amount, MaxEnergy);
        BroadcastResourceChange(EBioResourceType::Energy);
        break;
    default: break;
    }
}

void ABioResourceManager::AddBiomass(float Amount)
{
    Biomass = FMath::Min(Biomass + Amount, MaxBiomass);
    BroadcastResourceChange(EBioResourceType::Biomass);
}

void ABioResourceManager::AddGeneFragments(int32 Amount)
{
    GeneFragments = FMath::Min(GeneFragments + Amount, MaxGeneFragments);
    BroadcastResourceChange(EBioResourceType::GeneFragment);
}

void ABioResourceManager::SetBiomassIncome(float AmountPerSecond)
{
    BiomassIncome = FMath::Max(0.0f, AmountPerSecond);
}

void ABioResourceManager::UpgradeMaxStorage(EBioResourceType Type, float AdditionalAmount)
{
    switch (Type)
    {
    case EBioResourceType::Biomass:
        MaxBiomass += AdditionalAmount;
        break;
    case EBioResourceType::GeneFragment:
        MaxGeneFragments += FMath::RoundToInt(AdditionalAmount);
        break;
    case EBioResourceType::Energy:
        MaxEnergy += AdditionalAmount;
        break;
    default: break;
    }
    BroadcastResourceChange(Type);
}

void ABioResourceManager::ClampResource(EBioResourceType Type)
{
    switch (Type)
    {
    case EBioResourceType::Biomass:
        Biomass = FMath::Clamp(Biomass, 0.0f, MaxBiomass);
        break;
    case EBioResourceType::GeneFragment:
        GeneFragments = FMath::Clamp(GeneFragments, 0, MaxGeneFragments);
        break;
    case EBioResourceType::Energy:
        Energy = FMath::Clamp(Energy, 0.0f, MaxEnergy);
        break;
    default: break;
    }
}

void ABioResourceManager::BroadcastResourceChange(EBioResourceType Type)
{
    float CurrentAmount = GetResource(Type);
    OnResourceChanged.Broadcast(Type, CurrentAmount);
}
