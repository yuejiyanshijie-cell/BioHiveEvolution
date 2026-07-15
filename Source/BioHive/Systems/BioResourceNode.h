// BioResourceNode.h - 资源采集点
// 森林中可采集的资源节点

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BioResourceTypes.h"
#include "BioResourceNode.generated.h"

class ABioUnit;

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioResourceNode : public AActor
{
    GENERATED_BODY()

public:
    ABioResourceNode();

    // 采集资源
    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    float Harvest(float Amount);

    // 资源是否耗尽
    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    bool IsDepleted() const { return CurrentAmount <= 0.0f; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetCurrentAmount() const { return CurrentAmount; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetMaxAmount() const { return MaxAmount; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetHarvestRate() const { return HarvestRate; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    EBioResourceType GetResourceType() const { return ResourceType; }

    // 再生计时器
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

    // 资源类型
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bio|Resource")
    EBioResourceType ResourceType = EBioResourceType::Biomass;

    // 最大资源量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Resource",
        meta = (ClampMin = "10"))
    float MaxAmount = 1000.0f;

    // 当前资源量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Resource")
    float CurrentAmount = 1000.0f;

    // 每次采集量
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Resource",
        meta = (ClampMin = "1"))
    float HarvestRate = 10.0f;

    // 是否可自动再生
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Regeneration")
    bool bCanRegenerate = true;

    // 再生速率（每秒）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Regeneration",
        meta = (ClampMin = "0", EditCondition = "bCanRegenerate"))
    float RegenerationRate = 1.0f;

    // 再生延迟（耗尽后等待秒数）
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bio|Regeneration",
        meta = (ClampMin = "0", EditCondition = "bCanRegenerate"))
    float RegenerationDelay = 30.0f;

    // 特效
    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UNiagaraSystem* HarvestEffect;

    UPROPERTY(EditAnywhere, Category = "Bio|Visual")
    UNiagaraSystem* DepletedEffect;

    UPROPERTY(EditAnywhere, Category = "Bio|Sound")
    USoundBase* HarvestSound;

    // 采集点网格体
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bio|Components")
    UStaticMeshComponent* ResourceMesh;

private:
    float DepletionTimer = 0.0f;
    bool bWasDepleted = false;

    void StartRegeneration();
    void PlayHarvestEffects();
};
