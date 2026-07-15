// BioAIController.h - 生物母巢AI控制器
// 用于野生动物和敌人AI

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BioAIController.generated.h"

class UBehaviorTree;

// AI状态
UENUM(BlueprintType)
enum class EBioAIState : uint8
{
    Idle      UMETA(DisplayName = "闲逛"),
    Foraging  UMETA(DisplayName = "觅食"),
    Hunting   UMETA(DisplayName = "捕猎"),
    Fleeing   UMETA(DisplayName = "逃跑"),
    Attacking UMETA(DisplayName = "攻击"),
    Dead      UMETA(DisplayName = "死亡")
};

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioAIController : public AAIController
{
    GENERATED_BODY()

public:
    ABioAIController();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 切换AI状态
    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void SetAIState(EBioAIState NewState);

    UFUNCTION(BlueprintPure, Category = "Bio|AI")
    EBioAIState GetAIState() const { return CurrentAIState; }

    // 感知系统
    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

    UFUNCTION(BlueprintPure, Category = "Bio|AI")
    AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void SetTarget(AActor* NewTarget);

    // 巡逻
    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void StartPatrol(const TArray<FVector>& PatrolPoints);

    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void StopPatrol();

    // 逃跑
    UFUNCTION(BlueprintCallable, Category = "Bio|AI")
    void FleeFrom(AActor* Threat, float FleeDistance = 500.0f);

protected:
    // 行为树
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bio|AI")
    UBehaviorTree* DefaultBehaviorTree;

    // 巡逻点
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|AI")
    TArray<FVector> PatrolPoints;

    int32 CurrentPatrolIndex = 0;

    // 当前目标
    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentTarget;

    // AI状态
    EBioAIState CurrentAIState = EBioAIState::Idle;

    // 逃跑参数
    float FleeDistance = 500.0f;
    bool bIsFleeing = false;

    // 巡逻圈大小
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|AI")
    float PatrolRadius = 2000.0f;

    // 攻击范围
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|AI")
    float AttackRange = 300.0f;

    // 侦察范围
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|AI")
    float DetectionRange = 1500.0f;
};
