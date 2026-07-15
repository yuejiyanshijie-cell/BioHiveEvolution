// BioBuilding.h - 母巢建筑基类
// 所有建筑的基类：母巢核心、孵化池、进化腔、防御塔等

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BioResourceTypes.h"
#include "BioBuilding.generated.h"

class UWidgetComponent;

// 建筑类型枚举
UENUM(BlueprintType)
enum class EBioBuildingType : uint8
{
    HiveCore     UMETA(DisplayName = "母巢核心"),
    Hatchery     UMETA(DisplayName = "孵化池"),
    EvolutionChamber UMETA(DisplayName = "进化腔"),
    DefenseTower  UMETA(DisplayName = "防御塔"),
    ResourceCollector UMETA(DisplayName = "资源采集器"),
    SporeCannon   UMETA(DisplayName = "孢子炮台")
};

// 建筑状态
UENUM(BlueprintType)
enum class EBioBuildingState : uint8
{
    Placement     UMETA(DisplayName = "放置预览"),
    Constructing  UMETA(DisplayName = "建造中"),
    Active        UMETA(DisplayName = "运作中"),
    Damaged       UMETA(DisplayName = "受损"),
    Destroyed     UMETA(DisplayName = "已摧毁")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuildingStateChanged, EBioBuildingState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConstructionProgress, float, Progress);

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioBuilding : public AActor
{
    GENERATED_BODY()

public:
    ABioBuilding();

    virtual void Tick(float DeltaTime) override;

    // --- 建筑信息 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Building")
    EBioBuildingType GetBuildingType() const { return BuildingType; }

    UFUNCTION(BlueprintPure, Category = "Bio|Building")
    EBioBuildingState GetBuildingState() const { return CurrentState; }

    UFUNCTION(BlueprintPure, Category = "Bio|Building")
    FText GetBuildingName() const { return BuildingName; }

    // --- 建造系统 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void StartConstruction();

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void CompleteConstruction();

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void CancelConstruction();

    UFUNCTION(BlueprintPure, Category = "Bio|Building")
    float GetConstructionProgress() const;

    // 是否可以被放置在当前位置
    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    bool CanBePlacedAt(FVector Location) const;

    // --- 放置预览 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void SetPlacementMaterial(bool bCanPlace);

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void EnablePlacementMode();

    UFUNCTION(BlueprintCallable, Category = "Bio|Building")
    void DisablePlacementMode();

    // --- 生命值系统 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Health")
    float GetHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Bio|Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintCallable, Category = "Bio|Health")
    void TakeDamage(float DamageAmount);

    UFUNCTION(BlueprintCallable, Category = "Bio|Health")
    void Repair(float Amount);

    // --- 生产成本 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Cost")
    const TArray<FBioResourceCost>& GetBuildCosts() const { return BuildCosts; }

    // --- 事件 ---

    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnBuildingStateChanged OnBuildingStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnConstructionProgress OnConstructionProgress;

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Building")
    void OnConstructionComplete();

    UFUNCTION(BlueprintImplementableEvent, Category = "Bio|Building")
    void OnBuildingDestroyed();

    void SetState(EBioBuildingState NewState);

    // --- 网格体 ---

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    UStaticMeshComponent* BuildingMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    UWidgetComponent* HealthBarWidget;

    // --- 属性 ---

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Building")
    EBioBuildingType BuildingType;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Building")
    FText BuildingName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Building")
    FText BuildingDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Building")
    EBioBuildingState CurrentState = EBioBuildingState::Placement;

    // 建造时间（秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Construction",
        meta = (ClampMin = "1"))
    float ConstructionTime = 10.0f;

    // 当前建造进度
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Construction")
    float ConstructionProgress = 0.0f;

    // 建造消耗
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Cost")
    TArray<FBioResourceCost> BuildCosts;

    // 生命值
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Health",
        meta = (ClampMin = "1"))
    float MaxHealth = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Health")
    float CurrentHealth = 500.0f;

    // 放置验证范围
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Placement",
        meta = (ClampMin = "100"))
    float PlacementRadius = 300.0f;

    // 放置时材质
    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UMaterialInterface* ValidPlacementMaterial;

    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UMaterialInterface* InvalidPlacementMaterial;

    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UMaterialInterface* DefaultMaterial;

    // 建造粒子
    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UNiagaraSystem* ConstructionCompleteEffect;
};
