// BioUnitSelectionManager.cpp - 单位选择管理器实现

#include "BioUnitSelectionManager.h"
#include "BioUnit.h"
#include "BioResourceNode.h"
#include "BioBuilding.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

ABioUnitSelectionManager::ABioUnitSelectionManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABioUnitSelectionManager::BeginPlay()
{
    Super::BeginPlay();
}

void ABioUnitSelectionManager::SelectUnit(ABioUnit* Unit, bool bAddToSelection)
{
    if (!Unit) return;

    if (!bAddToSelection)
    {
        DeselectAll();
    }
    else if (SelectedUnits.Contains(Unit))
    {
        // Ctrl+点击已选中则取消选择
        Unit->Deselect();
        SelectedUnits.Remove(Unit);
        UpdateSelection();
        return;
    }

    Unit->Select();
    if (!SelectedUnits.Contains(Unit))
    {
        SelectedUnits.Add(Unit);
    }
    UpdateSelection();
}

void ABioUnitSelectionManager::SelectUnitsInRectangle(FVector2D Start, FVector2D End, bool bAddToSelection)
{
    if (!bAddToSelection)
    {
        DeselectAll();
    }

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    // 获取所有可选择的单位
    TArray<AActor*> AllUnits;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABioUnit::StaticClass(), AllUnits);

    for (AActor* Actor : AllUnits)
    {
        ABioUnit* Unit = Cast<ABioUnit>(Actor);
        if (!Unit) continue;

        FVector2D ScreenPos;
        if (PC->ProjectWorldLocationToScreen(Unit->GetActorLocation(), ScreenPos))
        {
            // 检查是否在框选区域内
            float MinX = FMath::Min(Start.X, End.X);
            float MaxX = FMath::Max(Start.X, End.X);
            float MinY = FMath::Min(Start.Y, End.Y);
            float MaxY = FMath::Max(Start.Y, End.Y);

            if (ScreenPos.X >= MinX && ScreenPos.X <= MaxX &&
                ScreenPos.Y >= MinY && ScreenPos.Y <= MaxY)
            {
                Unit->Select();
                SelectedUnits.AddUnique(Unit);
            }
        }
    }
    UpdateSelection();
}

void ABioUnitSelectionManager::SelectUnitsOfType(EBioUnitType Type)
{
    DeselectAll();

    TArray<AActor*> AllUnits;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABioUnit::StaticClass(), AllUnits);

    for (AActor* Actor : AllUnits)
    {
        ABioUnit* Unit = Cast<ABioUnit>(Actor);
        if (Unit && Unit->GetUnitType() == Type)
        {
            Unit->Select();
            SelectedUnits.Add(Unit);
        }
    }
    UpdateSelection();
}

void ABioUnitSelectionManager::SelectAllUnits()
{
    DeselectAll();

    TArray<AActor*> AllUnits;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABioUnit::StaticClass(), AllUnits);

    for (AActor* Actor : AllUnits)
    {
        ABioUnit* Unit = Cast<ABioUnit>(Actor);
        if (Unit)
        {
            Unit->Select();
            SelectedUnits.Add(Unit);
        }
    }
    UpdateSelection();
}

void ABioUnitSelectionManager::DeselectAll()
{
    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->Deselect();
        }
    }
    SelectedUnits.Empty();
    UpdateSelection();
}

void ABioUnitSelectionManager::IssueMoveCommand(FVector Location)
{
    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->MoveToLocation(Location);
        }
    }
}

void ABioUnitSelectionManager::IssueAttackCommand(AActor* Target)
{
    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->AttackTarget(Target);
        }
    }
}

void ABioUnitSelectionManager::IssueHarvestCommand(AActor* ResourceNode)
{
    ABioResourceNode* Node = Cast<ABioResourceNode>(ResourceNode);
    if (!Node) return;

    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->HarvestResource(Node);
        }
    }
}

void ABioUnitSelectionManager::IssueConsumeCommand(AActor* Target)
{
    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->ConsumeTarget(Target);
        }
    }
}

void ABioUnitSelectionManager::IssueHoldPositionCommand()
{
    for (ABioUnit* Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->HoldPosition();
        }
    }
}

void ABioUnitSelectionManager::IssueSmartCommand(AActor* Target, FVector Location)
{
    // 智能命令：根据目标类型自动判断操作
    if (ABioResourceNode* Node = Cast<ABioResourceNode>(Target))
    {
        IssueHarvestCommand(Target);
    }
    else if (ABioUnit* Unit = Cast<ABioUnit>(Target))
    {
        // 检查是否是敌人
        IssueAttackCommand(Target);
    }
    else if (ABioBuilding* Building = Cast<ABioBuilding>(Target))
    {
        // 检查是否是敌方建筑
        IssueAttackCommand(Target);
    }
    else
    {
        // 移动到位置
        IssueMoveCommand(Location);
    }
}

void ABioUnitSelectionManager::CreateFormation(int32 FormationKey)
{
    FBioUnitFormation NewFormation;
    NewFormation.FormationKey = FormationKey;
    NewFormation.Units = SelectedUnits;
    Formations.Add(FormationKey, NewFormation);
}

void ABioUnitSelectionManager::SelectFormation(int32 FormationKey)
{
    DeselectAll();

    FBioUnitFormation* Formation = Formations.Find(FormationKey);
    if (Formation)
    {
        for (ABioUnit* Unit : Formation->Units)
        {
            if (IsValid(Unit))
            {
                Unit->Select();
                SelectedUnits.Add(Unit);
            }
        }
        UpdateSelection();
    }
}

void ABioUnitSelectionManager::AddToFormation(int32 FormationKey, ABioUnit* Unit)
{
    FBioUnitFormation* Formation = Formations.Find(FormationKey);
    if (Formation && IsValid(Unit))
    {
        Formation->Units.AddUnique(Unit);
    }
}

bool ABioUnitSelectionManager::GetFormation(int32 FormationKey, TArray<ABioUnit*>& OutUnits) const
{
    const FBioUnitFormation* Formation = Formations.Find(FormationKey);
    if (Formation)
    {
        OutUnits = Formation->Units;
        return true;
    }
    return false;
}

void ABioUnitSelectionManager::UpdateSelection()
{
    // 移除无效单位
    SelectedUnits.RemoveAll([](ABioUnit* Unit) { return !IsValid(Unit); });
    OnSelectionChanged.Broadcast(SelectedUnits.Num());
}
