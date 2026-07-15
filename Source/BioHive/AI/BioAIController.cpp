// BioAIController.cpp - AI控制器实现

#include "BioAIController.h"
#include "BioUnit.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"

ABioAIController::ABioAIController()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABioAIController::BeginPlay()
{
    Super::BeginPlay();

    // 运行行为树
    if (DefaultBehaviorTree)
    {
        RunBehaviorTree(DefaultBehaviorTree);
    }
}

void ABioAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsFleeing)
    {
        // 检查是否已经逃出安全距离
        if (CurrentTarget.IsValid())
        {
            float Distance = FVector::Dist(
                GetPawn()->GetActorLocation(),
                CurrentTarget->GetActorLocation());

            if (Distance > FleeDistance)
            {
                bIsFleeing = false;
                SetAIState(EBioAIState::Idle);
            }
        }
        else
        {
            bIsFleeing = false;
            SetAIState(EBioAIState::Idle);
        }
    }
}

void ABioAIController::SetAIState(EBioAIState NewState)
{
    if (CurrentAIState != NewState)
    {
        CurrentAIState = NewState;

        // 更新黑板
        if (UBlackboardComponent* BB = GetBlackboardComponent())
        {
            BB->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(NewState));
        }
    }
}

void ABioAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    for (AActor* Actor : UpdatedActors)
    {
        ABioUnit* Unit = Cast<ABioUnit>(Actor);
        if (Unit && Unit != GetPawn())
        {
            CurrentTarget = Actor;
            SetAIState(EBioAIState::Hunting);
            MoveToActor(Actor);
            return;
        }
    }
}

void ABioAIController::SetTarget(AActor* NewTarget)
{
    CurrentTarget = NewTarget;
    if (UBlackboardComponent* BB = GetBlackboardComponent())
    {
        BB->SetValueAsObject(TEXT("TargetActor"), NewTarget);
    }
}

void ABioAIController::StartPatrol(const TArray<FVector>& InPatrolPoints)
{
    PatrolPoints = InPatrolPoints;
    CurrentPatrolIndex = 0;

    if (PatrolPoints.Num() > 0)
    {
        SetAIState(EBioAIState::Idle);
        MoveToLocation(PatrolPoints[CurrentPatrolIndex]);
    }
}

void ABioAIController::StopPatrol()
{
    StopMovement();
    PatrolPoints.Empty();
    SetAIState(EBioAIState::Idle);
}

void ABioAIController::FleeFrom(AActor* Threat, float InFleeDistance)
{
    if (!Threat || !GetPawn()) return;

    FleeDistance = InFleeDistance;
    bIsFleeing = true;
    CurrentTarget = Threat;
    SetAIState(EBioAIState::Fleeing);

    // 计算逃跑方向（远离威胁）
    FVector FleeDirection = GetPawn()->GetActorLocation() - Threat->GetActorLocation();
    FleeDirection.Normalize();
    FleeDirection.Z = 0;

    FVector FleeLocation = GetPawn()->GetActorLocation() + FleeDirection * FleeDistance;

    // 使用Navigation System确保目标点可达
    UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
    if (NavSystem)
    {
        FNavLocation NavLocation;
        if (NavSystem->GetRandomPointInNavigableRadius(FleeLocation, 500.0f, NavLocation))
        {
            MoveToLocation(NavLocation.Location);
        }
        else
        {
            MoveToLocation(FleeLocation);
        }
    }
}
