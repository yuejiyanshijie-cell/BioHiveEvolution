// BioItemSystem.cpp - 套装系统实现

#include "BioItemSystem.h"
#include "BioUnit.h"
#include "Engine/World.h"

UItemEquipmentComponent::UItemEquipmentComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UItemEquipmentComponent::BeginPlay()
{
    Super::BeginPlay();
    OwnerUnit = Cast<ABioUnit>(GetOwner());
}

bool UItemEquipmentComponent::EquipPiece(UItemPiece* Piece)
{
    if (!Piece) return false;

    EBioItemSlot Slot = Piece->Slot;

    // 先卸载当前槽位装备
    if (EquippedSlots.Contains(Slot))
    {
        RemoveAllModifiers(); // 先移除所有效果
        EquippedSlots[Slot] = Piece;
        RecalculateBonuses();
        ApplyModifiers();
    }
    else
    {
        RemoveAllModifiers();
        EquippedSlots.Add(Slot, Piece);
        RecalculateBonuses();
        ApplyModifiers();
    }

    OnEquipmentChanged.Broadcast();
    return true;
}

void UItemEquipmentComponent::UnequipSlot(EBioItemSlot Slot)
{
    if (EquippedSlots.Contains(Slot))
    {
        RemoveAllModifiers();
        EquippedSlots.Remove(Slot);
        RecalculateBonuses();
        ApplyModifiers();

        OnEquipmentChanged.Broadcast();
    }
}

UItemPiece* UItemEquipmentComponent::GetPieceInSlot(EBioItemSlot Slot) const
{
    if (const UItemPiece* const* Found = EquippedSlots.Find(Slot))
    {
        return *Found;
    }
    return nullptr;
}

int32 UItemEquipmentComponent::GetEquippedSetCount(FName SetName) const
{
    int32 Count = 0;
    for (const auto& Pair : EquippedSlots)
    {
        if (Pair.Value && Pair.Value->SetName == SetName)
        {
            Count++;
        }
    }
    return Count;
}

TArray<FSetBonus> UItemEquipmentComponent::GetActiveSetBonuses() const
{
    return ActiveBonuses;
}

TArray<FAttributeModifier> UItemEquipmentComponent::GetAllAttributeModifiers() const
{
    TArray<FAttributeModifier> AllModifiers;

    // 从装备部件收集属性
    for (const auto& Pair : EquippedSlots)
    {
        if (Pair.Value)
        {
            AllModifiers.Append(Pair.Value->Modifiers);
        }
    }

    // 从套装奖励收集属性
    for (const FSetBonus& Bonus : ActiveBonuses)
    {
        AllModifiers.Append(Bonus.AttributeModifiers);
    }

    return AllModifiers;
}

void UItemEquipmentComponent::UnequipAll()
{
    RemoveAllModifiers();
    EquippedSlots.Empty();
    ActiveBonuses.Empty();
    OnEquipmentChanged.Broadcast();
}

void UItemEquipmentComponent::EquipSet(UItemSet* Set)
{
    if (!Set) return;

    RemoveAllModifiers();

    // 先卸载所有装备
    UnequipAll();

    // 装备套装中的所有部件
    for (const TSoftObjectPtr<UItemPiece>& PiecePtr : Set->SetPieces)
    {
        UItemPiece* Piece = PiecePtr.LoadSynchronous();
        if (Piece)
        {
            EquippedSlots.Add(Piece->Slot, Piece);
        }
    }

    RecalculateBonuses();
    ApplyModifiers();
    OnEquipmentChanged.Broadcast();
}

void UItemEquipmentComponent::RecalculateBonuses()
{
    ActiveBonuses.Empty();

    // 按套装名称分组
    TMap<FName, int32> SetCounts;
    TMap<FName, UItemSet*> SetReferences;

    for (const auto& Pair : EquippedSlots)
    {
        if (Pair.Value)
        {
            SetCounts.FindOrAdd(Pair.Value->SetName)++;
            // 记录套装引用（从第一个部件获取）
            if (!SetReferences.Contains(Pair.Value->SetName))
            {
                // 这里需要从DataTable或AssetRegistry加载套装数据
                // 实际项目中需要有套装查询系统
            }
        }
    }

    // 为每个套装计算激活的奖励
    for (const auto& CountPair : SetCounts)
    {
        // 这里需要实际加载UItemSet数据资产来获取奖励
        // 为简化演示，记录已装备数量和套装名称
        if (CountPair.Value >= 2)
        {
            UE_LOG(LogTemp, Log, TEXT("Set '%s': %d pieces equipped"), *CountPair.Key.ToString(), CountPair.Value);
        }
    }
}

void UItemEquipmentComponent::ApplyModifiers()
{
    if (!OwnerUnit) return;

    TArray<FAttributeModifier> AllMods = GetAllAttributeModifiers();

    for (const FAttributeModifier& Mod : AllMods)
    {
        // 应用属性修改器到单位
        // 实际项目中通过GameplayAttribute或自定义系统应用
        UE_LOG(LogTemp, Log, TEXT("Applying modifier: %s = %.1f (Percent: %d)"),
            *Mod.AttributeName.ToString(), Mod.Value, Mod.bIsPercentBonus);
    }
}

void UItemEquipmentComponent::RemoveAllModifiers()
{
    if (!OwnerUnit) return;

    // 移除之前应用的所有修改器
    // 实际项目中需要保存修改器引用以便移除
    ActiveBonuses.Empty();
}
