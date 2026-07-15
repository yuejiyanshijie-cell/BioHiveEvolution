// BioRTSCamera.h - RTS视角相机
// 红色警戒风格的俯视相机，支持缩放和平移

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "BioRTSCamera.generated.h"

UCLASS(BlueprintType, Blueprintable)
class BIOHIVE_API ABioRTSCamera : public ACameraActor
{
    GENERATED_BODY()

public:
    ABioRTSCamera();

    virtual void Tick(float DeltaTime) override;

    // 相机移动输入
    void MoveForward(float Value);
    void MoveRight(float Value);
    void ZoomIn();
    void ZoomOut();

    // 边缘滚动输入
    void ProcessEdgeScrolling(float DeltaTime);

    // 聚焦到位置
    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void FocusOnLocation(FVector Location, float Duration = 0.5f);

    // 聚焦到Actor
    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void FocusOnActor(AActor* Target, float Duration = 0.5f);

    // 设置相机边界
    UFUNCTION(BlueprintCallable, Category = "Bio|Camera")
    void SetCameraBounds(FVector MinBounds, FVector MaxBounds);

protected:
    virtual void BeginPlay() override;

private:
    FVector ClampCameraLocation(const FVector& Location) const;
    void ApplyZoom(float Delta);

    // 相机参数
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Zoom")
    float MinZoom = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Zoom")
    float MaxZoom = 3000.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Zoom")
    float CurrentZoom = 1500.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Zoom")
    float ZoomSpeed = 500.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Zoom")
    float ZoomSmoothTime = 0.2f;

    // 移动参数
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Movement")
    float MovementSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Movement")
    float MovementSmoothTime = 0.15f;

    // 边缘滚动
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|EdgeScroll")
    bool bEnableEdgeScrolling = true;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|EdgeScroll",
        meta = (ClampMin = "0", ClampMax = "50"))
    float EdgeScrollThreshold = 20.0f;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|EdgeScroll")
    float EdgeScrollSpeed = 1000.0f;

    // 拖拽移动
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Drag")
    bool bEnableDragMove = true;

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Drag")
    float DragSpeed = 500.0f;

    // 相机旋转
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Rotation")
    FRotator DefaultRotation = FRotator(-45.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Rotation")
    float RotationSpeed = 100.0f;

    // 相机边界
    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Bounds")
    FVector CameraMinBounds = FVector(-10000, -10000, 0);

    UPROPERTY(EditAnywhere, Category = "Bio|Camera|Bounds")
    FVector CameraMaxBounds = FVector(10000, 10000, 5000);

    // 输入处理
    FVector2D CurrentInput;
    FVector2D SmoothedInput;
    FVector TargetLocation;

    // 聚焦动画
    bool bIsFocused = false;
    FVector FocusTarget;
    float FocusDuration;
    float FocusElapsed;

    // 拖拽状态
    bool bIsDragging = false;
    FVector2D LastMousePosition;
};
