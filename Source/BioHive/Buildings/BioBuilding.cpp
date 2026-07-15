// BioBuilding.cpp - 母巢建筑基类实现

#include "BioBuilding.h"
#include "BioResourceManager.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"

ABioBuilding::ABioBuilding()
{
    PrimaryActorTick.bCanEverTick = true;

    BuildingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BuildingMesh"));
    RootComponent = BuildingMesh;

    HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
    HealthBarWidget->SetupAttachment(RootComponent);
    HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
    HealthBarWidget->SetDrawSize(FVector2D(100.0f, 15.0f));
}

void ABioBuilding::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;

    if (CurrentState == EBioBuildingState::Placement)
    {
        EnablePlacementMode();
    }
}

void ABioBuilding::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (CurrentState == EBioBuildingState::Constructing)
    {
        ConstructionProgress += DeltaTime / ConstructionTime;
        OnConstructionProgress.Broadcast(ConstructionProgress);

        if (ConstructionProgress >= 1.0f)
        {
            CompleteConstruction();
        }
    }
}

bool ABioBuilding::CanBePlacedAt(FVector Location) const
{
    // 检查是否在其他建筑范围内
    // 检查地面是否平坦
    // 检查是否在母巢领地内

    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    FHitResult Hit;
    FVector Start = Location + FVector(0, 0, 500);
    FVector End = Location - FVector(0, 0, 500);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
    {
        float SlopeAngle = FMath::RadiansToDegrees(
            FMath::Acos(FVector::DotProduct(Hit.Normal, FVector::UpVector)));
        return SlopeAngle < 30.0f; // 坡度不超过30度
    }

    return false;
}

void ABioBuilding::SetPlacementMaterial(bool bCanPlace)
{
    if (!BuildingMesh) return;

    if (bCanPlace && ValidPlacementMaterial)
    {
        BuildingMesh->SetMaterial(0, ValidPlacementMaterial);
    }
    else if (!bCanPlace && InvalidPlacementMaterial)
    {
        BuildingMesh->SetMaterial(0, InvalidPlacementMaterial);
    }
}

void ABioBuilding::EnablePlacementMode()
{
    SetState(EBioBuildingState::Placement);
    SetPlacementMaterial(false);
    BuildingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABioBuilding::DisablePlacementMode()
{
    if (CurrentState == EBioBuildingState::Placement)
    {
        StartConstruction();
    }
}

void ABioBuilding::StartConstruction()
{
    ABioResourceManager* RM = ABioResourceManager::Get(this);
    if (!RM || !RM->CanAfford(BuildCosts))
    {
        // 资源不足，无法建造
        SetPlacementMaterial(false);
        return;
    }

    if (RM->SpendResources(BuildCosts))
    {
        SetState(EBioBuildingState::Constructing);
        ConstructionProgress = 0.0f;

        // 恢复默认材质
        if (DefaultMaterial && BuildingMesh)
        {
            BuildingMesh->SetMaterial(0, DefaultMaterial);
        }
        BuildingMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

void ABioBuilding::CompleteConstruction()
{
    ConstructionProgress = 1.0f;
    SetState(EBioBuildingState::Active);

    // 播放建造完成特效
    if (ConstructionCompleteEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), ConstructionCompleteEffect, GetActorLocation());
    }

    OnConstructionComplete();
}

void ABioBuilding::CancelConstruction()
{
    if (CurrentState == EBioBuildingState::Constructing)
    {
        // 退还50%资源
        ABioResourceManager* RM = ABioResourceManager::Get(this);
        if (RM)
        {
            for (const FBioResourceCost& Cost : BuildCosts)
            {
                RM->AddResource(Cost.ResourceType, Cost.Amount * 0.5f);
            }
        }
        Destroy();
    }
}

float ABioBuilding::GetConstructionProgress() const
{
    return ConstructionProgress;
}

void ABioBuilding::TakeDamage(float DamageAmount)
{
    if (CurrentState != EBioBuildingState::Active &&
        CurrentState != EBioBuildingState::Damaged) return;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

    if (CurrentHealth <= 0.0f)
    {
        SetState(EBioBuildingState::Destroyed);
        OnBuildingDestroyed();
        Destroy();
    }
    else if (CurrentHealth < MaxHealth * 0.5f)
    {
        SetState(EBioBuildingState::Damaged);
    }
}

void ABioBuilding::Repair(float Amount)
{
    if (CurrentState != EBioBuildingState::Damaged &&
        CurrentState != EBioBuildingState::Active) return;

    CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);

    if (CurrentHealth >= MaxHealth * 0.75f)
    {
        SetState(EBioBuildingState::Active);
    }
}

void ABioBuilding::SetState(EBioBuildingState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnBuildingStateChanged.Broadcast(NewState);
    }
}
