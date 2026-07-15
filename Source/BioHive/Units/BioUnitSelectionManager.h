// BioUnitSelectionManager.h - 单位选择管理器
// 类似红色警戒的框选和编队系统

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BioUnitSelectionManager.generated.h"

class ABioUnit;
class ABioRTSCamera;

USTRUCT(BlueprintType)
struct FBioUnitFormation
{
    GENERATED_BODY()

    UPROPERTY()
    int32 FormationKey = 0;

    UPROPERTY()
    TArray<ABioUnit*> Units;
};

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioUnitSelectionManager : public AActor
{
    GENERATED_BODY()

public:
    ABioUnitSelectionManager();

    // --- 选择操作 ---

    // 点选单个单位
    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void SelectUnit(ABioUnit* Unit, bool bAddToSelection = false);

    // 框选多个单位
    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void SelectUnitsInRectangle(FVector2D Start, FVector2D End, bool bAddToSelection = false);

    // 双击选择同类型
    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void SelectUnitsOfType(EBioUnitType Type);

    // 选择所有单位
    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void SelectAllUnits();

    // 取消所有选择
    UFUNCTION(BlueprintCallable, Category = "Bio|Selection")
    void DeselectAll();

    // 获取当前选中
    UFUNCTION(BlueprintPure, Category = "Bio|Selection")
    const TArray<ABioUnit*>& GetSelectedUnits() const { return SelectedUnits; }

    UFUNCTION(BlueprintPure, Category = "Bio|Selection")
    int32 GetSelectedCount() const { return SelectedUnits.Num(); }

    // --- 命令下发 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueMoveCommand(FVector Location);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueAttackCommand(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueHarvestCommand(AActor* ResourceNode);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueConsumeCommand(AActor* Target);

    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueHoldPositionCommand();

    // 智能命令（右键自动判断）
    UFUNCTION(BlueprintCallable, Category = "Bio|Command")
    void IssueSmartCommand(AActor* Target, FVector Location);

    // --- 编队系统 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Formation")
    void CreateFormation(int32 FormationKey);

    UFUNCTION(BlueprintCallable, Category = "Bio|Formation")
    void SelectFormation(int32 FormationKey);

    UFUNCTION(BlueprintCallable, Category = "Bio|Formation")
    void AddToFormation(int32 FormationKey, ABioUnit* Unit);

    UFUNCTION(BlueprintPure, Category = "Bio|Formation")
    bool GetFormation(int32 FormationKey, TArray<ABioUnit*>& OutUnits) const;

    // --- 事件 ---

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionChanged, int32, SelectedCount);
    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnSelectionChanged OnSelectionChanged;

protected:
    virtual void BeginPlay() override;

private:
    void UpdateSelection();

    // 当前选中的单位
    UPROPERTY()
    TArray<ABioUnit*> SelectedUnits;

    // 编队存储（Ctrl+数字键）
    UPROPERTY()
    TMap<int32, FBioUnitFormation> Formations;

    // 框选时的起始点
    FVector2D SelectionStartPoint;
    bool bIsSelecting = false;
};
