// BioResourceManager.h - 全局资源管理器
// 管理所有资源类型的存储、消耗和获取

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BioResourceTypes.h"
#include "BioResourceManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioResourceManager : public AActor
{
    GENERATED_BODY()

public:
    ABioResourceManager();

    // 获取单例
    UFUNCTION(BlueprintCallable, Category = "Bio|Resource",
        meta = (WorldContext = "WorldContextObject"))
    static ABioResourceManager* Get(const UObject* WorldContextObject);

public:
    // --- 资源获取 ---

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetResource(EBioResourceType Type) const;

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetBiomass() const { return Biomass; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    int32 GetGeneFragments() const { return GeneFragments; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetEnergy() const { return Energy; }

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetMaxStorage(EBioResourceType Type) const;

    // --- 资源消耗 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    bool SpendResources(const TArray<FBioResourceCost>& Costs);

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    bool SpendBiomass(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    bool SpendGeneFragments(int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    bool SpendEnergy(float Amount);

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    bool CanAfford(const TArray<FBioResourceCost>& Costs) const;

    // --- 资源增加 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    void AddResource(EBioResourceType Type, float Amount);

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    void AddBiomass(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    void AddGeneFragments(int32 Amount);

    // --- 资源生产 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    void SetBiomassIncome(float AmountPerSecond);

    UFUNCTION(BlueprintPure, Category = "Bio|Resource")
    float GetBiomassIncome() const { return BiomassIncome; }

    // --- 存储升级 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Resource")
    void UpgradeMaxStorage(EBioResourceType Type, float AdditionalAmount);

    // --- 事件 ---

    UPROPERTY(BlueprintAssignable, Category = "Bio|Event")
    FOnResourceChanged OnResourceChanged;

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    void ClampResource(EBioResourceType Type);
    void BroadcastResourceChange(EBioResourceType Type);

    // 主要资源
    UPROPERTY(EditAnywhere, Category = "Bio|Resource",
        meta = (ClampMin = "0"))
    float Biomass = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Resource",
        meta = (ClampMin = "0"))
    int32 GeneFragments = 0;

    UPROPERTY(EditAnywhere, Category = "Bio|Resource",
        meta = (ClampMin = "0"))
    float Energy = 100.0f;

    // 资源上限
    UPROPERTY(EditAnywhere, Category = "Bio|Storage",
        meta = (ClampMin = "0"))
    float MaxBiomass = 5000.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Storage",
        meta = (ClampMin = "0"))
    int32 MaxGeneFragments = 100;

    UPROPERTY(EditAnywhere, Category = "Bio|Storage",
        meta = (ClampMin = "0"))
    float MaxEnergy = 200.0f;

    // 自动收入
    UPROPERTY(EditAnywhere, Category = "Bio|Income")
    float BiomassIncome = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Income")
    float EnergyIncome = 0.0f;
};
