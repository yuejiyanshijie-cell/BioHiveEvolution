// BioItemSystem.h - 套装系统
// 替代基因组合系统，采用类似暗黑破坏神的套装机制

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BioItemSystem.generated.h"

// 套装类型（对应三大进化路线）
UENUM(BlueprintType)
enum class EBioSetType : uint8
{
    Predator  UMETA(DisplayName = "捕食者"),   // 红色系 - 攻击型
    Defender  UMETA(DisplayName = "防御者"),   // 蓝色系 - 坦克型
    Support   UMETA(DisplayName = "辅助者"),   // 绿色系 - 控制/辅助型
    Legendary UMETA(DisplayName = "传说")       // 特殊效果
};

// 套装槽位
UENUM(BlueprintType)
enum class EBioItemSlot : uint8
{
    Core       UMETA(DisplayName = "核心"),
    Weapon     UMETA(DisplayName = "武器"),
    Armor      UMETA(DisplayName = "装甲"),
    Sensor     UMETA(DisplayName = "感应器"),
    Generator  UMETA(DisplayName = "发生器"),
    Movement   UMETA(DisplayName = "移动器官")
};

// 属性修改器
USTRUCT(BlueprintType)
struct FBIOHIVE_API FAttributeModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName AttributeName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPercentBonus = false; // true=百分比加成, false=固定值
};

// 套装奖励
USTRUCT(BlueprintType)
struct FBIOHIVE_API FSetBonus
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText BonusName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText BonusDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FAttributeModifier> AttributeModifiers;

    // 特殊效果ID（在蓝图中实现）
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SpecialEffectID;
};

// 套装部件
UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API UItemPiece : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FName PieceName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon;

    // 所属套装
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    FName SetName;

    // 所属套装类型
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    EBioSetType SetType;

    // 槽位
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    EBioItemSlot Slot;

    // 部件等级 I, II, III
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set",
        meta = (ClampMin = "1", ClampMax = "5"))
    int32 Tier = 1;

    // 属性
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
    TArray<FAttributeModifier> Modifiers;

    // 稀有度（影响掉落概率）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats",
        meta = (ClampMin = "1", ClampMax = "5"))
    int32 Rarity = 1;
};

// 套装定义
UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API UItemSet : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    FName SetName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    UTexture2D* SetIcon;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    EBioSetType SetType;

    // 套装部件列表
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Set")
    TArray<TSoftObjectPtr<UItemPiece>> SetPieces;

    // 2件套奖励
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses")
    FSetBonus TwoPieceBonus;

    // 3件套奖励
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses")
    FSetBonus ThreePieceBonus;

    // 4件套奖励（如果套装是4件）
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses")
    FSetBonus FourPieceBonus;

    // 满套奖励
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bonuses")
    FSetBonus FullSetBonus;
};

// 装备管理器组件
UCLASS(ClassGroup = (BioHive), meta = (BlueprintSpawnableComponent))
class BIOHIVE_API UItemEquipmentComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UItemEquipmentComponent();

    // 装备部件
    UFUNCTION(BlueprintCallable, Category = "Bio|Equipment")
    bool EquipPiece(UItemPiece* Piece);

    // 卸载部件
    UFUNCTION(BlueprintCallable, Category = "Bio|Equipment")
    void UnequipSlot(EBioItemSlot Slot);

    // 获取槽位中的部件
    UFUNCTION(BlueprintPure, Category = "Bio|Equipment")
    UItemPiece* GetPieceInSlot(EBioItemSlot Slot) const;

    // 获取某套装已装备数量
    UFUNCTION(BlueprintPure, Category = "Bio|Equipment")
    int32 GetEquippedSetCount(FName SetName) const;

    // 获取当前激活的套装奖励
    UFUNCTION(BlueprintPure, Category = "Bio|Equipment")
    TArray<FSetBonus> GetActiveSetBonuses() const;

    // 获取所有属性加成
    UFUNCTION(BlueprintPure, Category = "Bio|Equipment")
    TArray<FAttributeModifier> GetAllAttributeModifiers() const;

    // 一键卸下所有装备
    UFUNCTION(BlueprintCallable, Category = "Bio|Equipment")
    void UnequipAll();

    // 一键装备套装
    UFUNCTION(BlueprintCallable, Category = "Bio|Equipment")
    void EquipSet(UItemSet* Set);

protected:
    virtual void BeginPlay() override;

private:
    void RecalculateBonuses();
    void ApplyModifiers();
    void RemoveAllModifiers();

    // 装备槽位映射
    UPROPERTY()
    TMap<EBioItemSlot, UItemPiece*> EquippedSlots;

    // 当前激活的奖励
    UPROPERTY()
    TArray<FSetBonus> ActiveBonuses;

    // 装备变化事件
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);
    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnEquipmentChanged OnEquipmentChanged;

    UPROPERTY()
    class ABioUnit* OwnerUnit;
};
