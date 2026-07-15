// BioHiveHUD.h - 简单HUD显示生物质和器官信息
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BioHiveHUD.generated.h"

UCLASS()
class BIOHIVE_API ABioHiveHUD : public AHUD
{
    GENERATED_BODY()

public:
    ABioHiveHUD();

    virtual void DrawHUD() override;

    // 设置要显示的生物质
    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetBiomassDisplay(float NewBiomass);

    UFUNCTION(BlueprintCallable, Category = "UI")
    void SetOrgansDisplay(int32 Organs);

protected:
    virtual void BeginPlay() override;

private:
    float DisplayBiomass = 0.0f;
    int32 DisplayOrgans = 0;

    // UI字体
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    class UFont* HUDFont;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    int32 FontSize = 24;
};
