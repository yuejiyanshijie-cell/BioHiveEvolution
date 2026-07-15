// BioResourceTypes.h - 资源类型定义
// 生物母巢进化游戏 - 核心资源系统

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BioResourceTypes.generated.h"

// 资源类型枚举
UENUM(BlueprintType)
enum class EBioResourceType : uint8
{
    Biomass     UMETA(DisplayName = "生物质"),   // 主要资源
    GeneFragment UMETA(DisplayName = "基因片段"), // 稀有资源
    Energy      UMETA(DisplayName = "能量"),      // 高级资源
    OrganicMatter UMETA(DisplayName = "有机物质")  // 基础材料
};

// 资源数据表行
USTRUCT(BlueprintType)
struct FBioResourceData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBioResourceType ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor UIColor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxStorage;
};

// 资源购买请求
USTRUCT(BlueprintType)
struct FBioResourceCost
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBioResourceType ResourceType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Amount;
};

// 资源变动事件（用于UI绑定）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceChanged, EBioResourceType, ResourceType, float, NewAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSyncNetMessage);
