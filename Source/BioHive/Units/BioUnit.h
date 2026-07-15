// BioUnit.h - 生物母巢单位基类
// 所有可控制单位的基类：工蜂、战士、喷射兽等

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BioUnit.generated.h"

class ABioResourceNode;
class UItemSet;
class UWidgetComponent;

// 单位类型
UENUM(BlueprintType)
enum class EBioUnitType : uint8
{
    Drone      UMETA(DisplayName = "工蜂"),
    Warrior    UMETA(DisplayName = "战士"),
    Hydralisk  UMETA(DisplayName = "喷射兽"),
    Flyer      UMETA(DisplayName = "飞行者"),
    Tank       UMETA(DisplayName = "坦克兽"),
    Queen      UMETA(DisplayName = "蜂后")
};

// 单位状态
UENUM(BlueprintType)
enum class EBioUnitState : uint8
{
    Idle         UMETA(DisplayName = "待机"),
    Moving       UMETA(DisplayName = "移动中"),
    Harvesting   UMETA(DisplayName = "采集中"),
    Attacking    UMETA(DisplayName = "攻击中"),
    Consuming    UMETA(DisplayName = "吞噬中"),
    Dead         UMETA(DisplayName = "死亡")
};

// 单位命令
UENUM(BlueprintType)
enum class EBioUnitCommand : uint8
{
    Move         UMETA(DisplayName = "移动"),
    Attack       UMETA(DisplayName = "攻击"),
    Harvest      UMETA(DisplayName = "采集"),
    Consume      UMETA(DisplayName = "吞噬"),
    HoldPosition UMETA(DisplayName = "坚守"),
    Patrol       UMETA(DisplayName = "巡逻"),
    Retreat      UMETA(DisplayName = "撤退")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitSelected, ABioUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitDeselected, ABioUnit*, Unit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUnitStateChanged, EBioUnitState, NewState);

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioUnit : public ACharacter
{
    GENERATED_BODY()

public:
    ABioUnit();

    virtual void Tick(float DeltaTime) override;

    // --- 单位信息 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Unit")
    EBioUnitType GetUnitType() const { return UnitType; }

    UFUNCTION(BlueprintPure, Category = "Bio|Unit")
    EBioUnitState GetUnitState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Bio|Unit")
    FText GetUnitName() const { return UnitName; }

    // --- RTS命令 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueCommand(EBioUnitCommand Command, AActor* Target = nullptr, FVector Location = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void MoveToLocation(FVector TargetLocation);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void AttackTarget(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void HarvestResource(ABioResourceNode* ResourceNode);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void ConsumeTarget(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void HoldPosition();

    // --- 选择系统 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void Select();

    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void Deselect();

    UFUNCTION(BlueprintPure, Category = "Bio|Selection")
    bool IsSelected() const { return bIsSelected; }

    // --- 视角切换 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void EnterFirstPersonView();

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void ExitFirstPersonView();

    UFUNCTION(BlueprintPure, Category = "Bio|Camera")
    bool IsFirstPersonView() const { return bInFirstPerson; }

    // --- 战斗系统 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Combat")
    void TakeDamage(float DamageAmount, AActor* DamageCauser);

    UFUNCTION(BlueprintPure, Category = "Bio|Combat")
    float GetHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Bio|Combat")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Bio|Combat")
    float GetAttackDamage() const { return BaseAttackDamage * AttackDamageMultiplier; }

    // --- 吞噬系统 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Consume")
    float GetConsumeProgress() const { return ConsumeProgress; }

    UFUNCTION(BlueprintCallable, Category = "Bio|Consume")
    void InterruptConsume();

    // --- 属性 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Stats")
    int32 GetUnitLevel() const { return UnitLevel; }

    UFUNCTION(BlueprintPure, Category = "Bio|Stats")
    int32 GetUnitTier() const { return UnitTier; }

    // --- 事件 ---

    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnUnitStateChanged OnUnitStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnUnitStateChanged OnUnitDeath;

protected:
    virtual void BeginPlay() override;

    // 蓝图事件
    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Unit")
    void OnCommandReceived(EBioUnitCommand Command);

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Unit")
    void OnAttackExecuted(AActor* Target);

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Unit")
    void OnConsumeComplete(AActor* Target);

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Unit")
    void OnDeath();

    void SetState(EBioUnitState NewState);

    // 私有辅助
    void PerformAttack(AActor* Target);
    void PerformConsume(AActor* Target);
    void Die();

    // --- 组件 ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    USkeletalMeshComponent* UnitMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    UWidgetComponent* SelectionIndicator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    UWidgetComponent* HealthBar;

    // --- 单位属性 ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Unit")
    EBioUnitType UnitType = EBioUnitType::Drone;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Unit")
    FText UnitName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Unit")
    int32 UnitLevel = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Unit")
    int32 UnitTier = 1;  // I, II, III

    // 战斗属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Combat")
    float MaxHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Combat")
    float CurrentHealth = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Combat")
    float BaseAttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Combat",
        meta = (ClampMin = "0.5"))
    float AttackSpeed = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Combat",
        meta = (ClampMin = "50"))
    float AttackRange = 200.0f;

    // 采集属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Harvest")
    float HarvestRate = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Harvest")
    float HarvestRange = 150.0f;

    // 吞噬属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Consume")
    float ConsumeDuration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Consume")
    float ConsumeRange = 150.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Consume")
    float MinTargetHealthPercent = 0.3f; // 目标血量低于30%才能吞噬

    // 移动属性
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Movement")
    float BaseMovementSpeed = 600.0f;

    // 生产和消耗
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Cost")
    TArray<FBioResourceCost> ProductionCosts;

    // --- 状态 ---

    EBioUnitState CurrentState = EBioUnitState::Idle;
    bool bIsSelected = false;
    bool bInFirstPerson = false;

    // 属性倍率（用于套装加成）
    float AttackDamageMultiplier = 1.0f;
    float MovementSpeedMultiplier = 1.0f;
    float HealthMultiplier = 1.0f;

    // 吞噬
    float ConsumeProgress = 0.0f;
    TWeakObjectPtr<AActor> ConsumeTarget;

    // 定时器
    FTimerHandle AttackTimerHandle;
    FTimerHandle ConsumeTimerHandle;
    FTimerHandle HarvestTimerHandle;
};
