// BioUnit.cpp - 生物母巢单位基类实现

#include "BioUnit.h"
#include "BioResourceNode.h"
#include "BioResourceManager.h"
#include "BioBuilding.h"
#include "NavigationSystem.h"
#include "Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

ABioUnit::ABioUnit()
{
    PrimaryActorTick.bCanEverTick = true;

    UnitMesh = GetMesh();
    if (UnitMesh)
    {
        UnitMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }

    SelectionIndicator = CreateDefaultSubobject<UWidgetComponent>(TEXT("SelectionIndicator"));
    SelectionIndicator->SetupAttachment(RootComponent);
    SelectionIndicator->SetWidgetSpace(EWidgetSpace::Screen);
    SelectionIndicator->SetVisibility(false);

    HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBar->SetupAttachment(RootComponent);
    HealthBar->SetWidgetSpace(EWidgetSpace::Screen);

    // 默认移动速度
    if (UCharacterMovementComponent* Movement = GetCharacterMovement())
    {
        Movement->MaxWalkSpeed = BaseMovementSpeed;
    }
}

void ABioUnit::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;
    BaseMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
}

void ABioUnit::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABioUnit::IssueCommand(EBioUnitCommand Command, AActor* Target, FVector Location)
{
    switch (Command)
    {
    case EBioUnitCommand::Move:
        MoveToLocation(Location);
        break;
    case EBioUnitCommand::Attack:
        AttackTarget(Target);
        break;
    case EBioUnitCommand::Harvest:
        HarvestResource(Cast<ABioResourceNode>(Target));
        break;
    case EBioUnitCommand::Consume:
        ConsumeTarget(Target);
        break;
    case EBioUnitCommand::HoldPosition:
        HoldPosition();
        break;
    default: break;
    }
    OnCommandReceived(Command);
}

void ABioUnit::MoveToLocation(FVector TargetLocation)
{
    if (CurrentState == EBioUnitState::Consuming)
    {
        InterruptConsume();
    }

    // 使用AI移动
    AController* Controller = GetController();
    if (AAIController* AIC = Cast<AAIController>(Controller))
    {
        AIC->MoveToLocation(TargetLocation);
    }
    else
    {
        UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, TargetLocation);
    }

    SetState(EBioUnitState::Moving);
}

void ABioUnit::AttackTarget(AActor* Target)
{
    if (!Target || CurrentState == EBioUnitState::Dead) return;
    if (CurrentState == EBioUnitState::Consuming) InterruptConsume();

    SetState(EBioUnitState::Attacking);

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > AttackRange)
    {
        MoveToLocation(Target->GetActorLocation());
    }
    else if (!AttackTimerHandle.IsValid())
    {
        GetWorldTimerManager().SetTimer(AttackTimerHandle,
            FTimerDelegate::CreateUObject(this, &ABioUnit::PerformAttack, Target),
            1.0f / AttackSpeed, true);
    }
}

void ABioUnit::HarvestResource(ABioResourceNode* ResourceNode)
{
    if (!ResourceNode || CurrentState == EBioUnitState::Dead) return;

    SetState(EBioUnitState::Harvesting);

    float Distance = FVector::Dist(GetActorLocation(), ResourceNode->GetActorLocation());
    if (Distance > HarvestRange)
    {
        MoveToLocation(ResourceNode->GetActorLocation());
    }
    else if (!HarvestTimerHandle.IsValid())
    {
        GetWorldTimerManager().SetTimer(HarvestTimerHandle,
            [this, ResourceNode]()
            {
                if (ResourceNode && !ResourceNode->IsDepleted())
                {
                    ResourceNode->Harvest(HarvestRate);
                }
            },
            0.5f, true);
    }
}

void ABioUnit::ConsumeTarget(AActor* Target)
{
    if (!Target || CurrentState == EBioUnitState::Dead) return;

    ABioUnit* TargetUnit = Cast<ABioUnit>(Target);
    if (TargetUnit)
    {
        // 检查目标血量
        float TargetHealthPercent = TargetUnit->GetHealth() / TargetUnit->GetMaxHealth();
        if (TargetHealthPercent > MinTargetHealthPercent)
        {
            // 目标血量太高，先攻击
            AttackTarget(Target);
            return;
        }
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > ConsumeRange)
    {
        MoveToLocation(Target->GetActorLocation());
    }
    else
    {
        SetState(EBioUnitState::Consuming);
        ConsumeTarget = Target;
        ConsumeProgress = 0.0f;

        GetWorldTimerManager().SetTimer(ConsumeTimerHandle, this,
            &ABioUnit::PerformConsume, ConsumeDuration, false);
    }
}

void ABioUnit::HoldPosition()
{
    if (AController* Controller = GetController())
    {
        Controller->StopMovement();
    }
    SetState(EBioUnitState::Idle);
}

void ABioUnit::Select()
{
    bIsSelected = true;
    if (SelectionIndicator)
    {
        SelectionIndicator->SetVisibility(true);
    }
}

void ABioUnit::Deselect()
{
    bIsSelected = false;
    if (SelectionIndicator)
    {
        SelectionIndicator->SetVisibility(false);
    }
}

void ABioUnit::EnterFirstPersonView()
{
    bInFirstPerson = true;
    // 此功能由PlayerController处理
}

void ABioUnit::ExitFirstPersonView()
{
    bInFirstPerson = false;
}

void ABioUnit::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
    if (CurrentState == EBioUnitState::Dead) return;

    CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);

    if (CurrentHealth <= 0.0f)
    {
        Die();
    }
}

void ABioUnit::InterruptConsume()
{
    if (CurrentState == EBioUnitState::Consuming)
    {
        ConsumeProgress = 0.0f;
        ConsumeTarget.Reset();
        GetWorldTimerManager().ClearTimer(ConsumeTimerHandle);
        SetState(EBioUnitState::Idle);
    }
}

void ABioUnit::PerformAttack(AActor* Target)
{
    if (!Target || CurrentState == EBioUnitState::Dead)
    {
        GetWorldTimerManager().ClearTimer(AttackTimerHandle);
        return;
    }

    float Distance = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Distance > AttackRange)
    {
        MoveToLocation(Target->GetActorLocation());
        return;
    }

    // 对目标造成伤害
    if (ABioUnit* TargetUnit = Cast<ABioUnit>(Target))
    {
        TargetUnit->TakeDamage(GetAttackDamage(), this);

        // 如果目标死亡，自动吞噬
        if (TargetUnit->GetHealth() <= 0)
        {
            GetWorldTimerManager().ClearTimer(AttackTimerHandle);
            ConsumeTarget(Target);
        }
    }
    else if (ABioBuilding* TargetBuilding = Cast<ABioBuilding>(Target))
    {
        TargetBuilding->TakeDamage(GetAttackDamage());
    }

    OnAttackExecuted(Target);
}

void ABioUnit::PerformConsume(AActor* Target)
{
    if (!Target || CurrentState == EBioUnitState::Dead) return;

    ConsumeProgress = 1.0f;

    // 根据目标类型获得资源
    if (ABioUnit* TargetUnit = Cast<ABioUnit>(Target))
    {
        float BiomassGain = TargetUnit->MaxHealth * 0.5f;
        ABioResourceManager* RM = ABioResourceManager::Get(this);
        if (RM) RM->AddBiomass(BiomassGain);

        // 有概率获得基因片段
        if (FMath::FRand() < 0.1f)
        {
            if (RM) RM->AddGeneFragments(1);
        }
    }

    OnConsumeComplete(Target);

    // 销毁目标
    Target->Destroy();
    SetState(EBioUnitState::Idle);
}

void ABioUnit::Die()
{
    SetState(EBioUnitState::Dead);
    OnUnitDeath.Broadcast(CurrentState);
    OnDeath();

    // 延迟销毁
    SetLifeSpan(5.0f);
}

void ABioUnit::SetState(EBioUnitState NewState)
{
    if (CurrentState != NewState)
    {
        CurrentState = NewState;
        OnUnitStateChanged.Broadcast(NewState);
    }
}
