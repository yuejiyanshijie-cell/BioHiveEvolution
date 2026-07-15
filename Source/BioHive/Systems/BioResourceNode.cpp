// BioResourceNode.cpp - 资源采集点实现

#include "BioResourceNode.h"
#include "BioResourceManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"

ABioResourceNode::ABioResourceNode()
{
    PrimaryActorTick.bCanEverTick = true;

    ResourceMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ResourceMesh"));
    RootComponent = ResourceMesh;
}

void ABioResourceNode::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmount = MaxAmount;

    if (DepletedEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            DepletedEffect, ResourceMesh, NAME_None,
            FVector::ZeroVector, FRotator::ZeroRotator,
            EAttachLocation::KeepRelativeOffset, false);
    }
}

float ABioResourceNode::Harvest(float Amount)
{
    if (IsDepleted()) return 0.0f;

    float Harvested = FMath::Min(Amount, CurrentAmount);
    CurrentAmount -= Harvested;

    PlayHarvestEffects();

    // 通知资源管理器
    if (ABioResourceManager* RM = ABioResourceManager::Get(this))
    {
        RM->AddResource(ResourceType, Harvested);
    }

    if (IsDepleted() && !bWasDepleted)
    {
        bWasDepleted = true;
        if (bCanRegenerate)
        {
            DepletionTimer = RegenerationDelay;
        }
        // 播放枯竭特效
        if (DepletedEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(), DepletedEffect, GetActorLocation());
        }
    }

    return Harvested;
}

void ABioResourceNode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bWasDepleted && bCanRegenerate)
    {
        DepletionTimer -= DeltaTime;
        if (DepletionTimer <= 0.0f)
        {
            StartRegeneration();
        }
    }
}

void ABioResourceNode::StartRegeneration()
{
    CurrentAmount += RegenerationRate * RegenerationDelay;
    CurrentAmount = FMath::Min(CurrentAmount, MaxAmount);

    if (CurrentAmount > MaxAmount * 0.1f)
    {
        bWasDepleted = false;
        DepletionTimer = 0.0f;
    }
}

void ABioResourceNode::PlayHarvestEffects()
{
    if (HarvestEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), HarvestEffect, GetActorLocation());
    }
    if (HarvestSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, HarvestSound, GetActorLocation());
    }
}
