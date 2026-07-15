// BioHiveCharacter.h - 第一人称生物母巢核心角色
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BioHiveCharacter.generated.h"

UCLASS()
class BIOHIVE_API ABioHiveCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABioHiveCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent() override;

    // 吞噬交互
    UFUNCTION(BlueprintCallable, Category = "Consume")
    void TryConsume();

    // 获取生物质
    UFUNCTION(BlueprintPure, Category = "Biomass")
    float GetBiomass() const { return Biomass; }

    // 进化器官
    UFUNCTION(BlueprintCallable, Category = "Evolution")
    bool EvolveOrgan(int32 OrganIndex);

    UFUNCTION(BlueprintPure, Category = "Evolution")
    int32 GetCurrentOrgans() const { return UnlockedOrgans; }

protected:
    virtual void BeginPlay() override;

    // 进化器官(带UI更新)
    UFUNCTION(BlueprintCallable, Category = "Evolution")
    bool EvolveOrganWithUI(int32 OrganIndex);

private:
    // 输入处理
    void MoveForward(float Value);
    void MoveRight(float Value);
    void LookUp(float Value);
    void Turn(float Value);

    // 吞噬逻辑
    void ConsumeTarget(AActor* Target);

    // 属性
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class UCameraComponent* FirstPersonCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USphereComponent* ConsumeSphere;

    // 资源
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
    float Biomass = 0.0f;

    // 器官系统 (位掩码)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evolution")
    int32 UnlockedOrgans = 0;

    // 器官成本配置
    UPROPERTY(EditDefaultsOnly, Category = "Evolution")
    TArray<float> OrganCosts;

    // 吞噬范围
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consume")
    float ConsumeRange = 300.0f;

    // 吞噬冷却
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Consume")
    float ConsumeCooldown = 1.0f;

    float LastConsumeTime = 0.0f;

    // HUD引用
    UPROPERTY()
    class ABioHiveHUD* BioHiveHUD;

    // 更新UI显示
    void UpdateHUD();
};
