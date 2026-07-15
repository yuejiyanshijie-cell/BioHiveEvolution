// BioHivePlayerController.h - RTS玩家控制器
// 管理输入、视角切换、命令下发

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BioHivePlayerController.generated.h"

class ABioRTSCamera;
class ABioUnitSelectionManager;
class ABioUnit;
class UInputMappingContext;
class UInputAction;

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioHivePlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    ABioHivePlayerController();

    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

    // --- 视角切换 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void ToggleViewMode();

    UFUNCTION(BlueprintPure, Category = "Bio|Camera")
    bool IsRTSMode() const { return bIsRTSMode; }

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void EnterFirstPersonMode(ABioUnit* Unit);

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void ExitFirstPersonMode();

    // --- 相机控制 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    ABioRTSCamera* GetRTSCamera() const { return RTSCamera; }

    // --- 交互 ---

    UFUNCTION(BlueprintCallable, Category = "Bio|Interaction")
    void OnClickPressed();

    UFUNCTION(BlueprintCallable, Category = "Bio|Interaction")
    void OnClickReleased();

    UFUNCTION(BlueprintCallable, Category = "Bio|Interaction")
    void OnRightClick();

protected:
    // 输入处理
    void Input_MoveForward(const FInputActionValue& Value);
    void Input_MoveRight(const FInputActionValue& Value);
    void Input_ZoomIn();
    void Input_ZoomOut();
    void Input_ToggleBuildMenu();
    void Input_SelectAll();
    void Input_HoldPosition();
    void Input_Formation(int32 Key);

    // 选择
    void PerformSelection();
    void PerformCommand(AActor* HitActor, FVector HitLocation);

    // 相机
    UPROPERTY()
    ABioRTSCamera* RTSCamera;

    bool bIsRTSMode = true;
    bool bIsSelecting = false;
    FVector2D SelectionStart;
    FVector2D SelectionEnd;

    // 第一人称控制的目标单位
    UPROPERTY()
    TWeakObjectPtr<ABioUnit> FirstPersonUnit;

    // 增强输入
    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputMappingContext* BioHiveMappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_MoveForward;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_MoveRight;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_Zoom;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_ToggleView;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_SelectClick;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_CommandClick;

    UPROPERTY(EditDefaultsOnly, Category = "Bio|Input")
    UInputAction* IA_BuildMenu;
};
