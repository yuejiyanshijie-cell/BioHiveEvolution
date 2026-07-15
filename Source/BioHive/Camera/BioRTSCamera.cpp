// BioRTSCamera.cpp - RTS视角相机实现

#include "BioRTSCamera.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABioRTSCamera::ABioRTSCamera()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABioRTSCamera::BeginPlay()
{
    Super::BeginPlay();

    SetActorRotation(DefaultRotation);
    TargetLocation = GetActorLocation();
    CurrentZoom = (MinZoom + MaxZoom) * 0.5f;

    // 设置初始位置
    FVector InitialPos = GetActorLocation();
    InitialPos.Z = CurrentZoom;
    SetActorLocation(InitialPos);
}

void ABioRTSCamera::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 处理边缘滚动
    if (bEnableEdgeScrolling)
    {
        ProcessEdgeScrolling(DeltaTime);
    }

    // 平滑化输入
    SmoothedInput = FMath::Vector2DInterpTo(
        SmoothedInput, CurrentInput, DeltaTime, 1.0f / MovementSmoothTime);

    // 计算新位置
    FVector NewLocation = GetActorLocation();

    if (bIsFocused)
    {
        FocusElapsed += DeltaTime;
        float Alpha = FMath::Clamp(FocusElapsed / FocusDuration, 0.0f, 1.0f);
        // 缓入缓出
        Alpha = FMath::SmoothStep(0.0f, 1.0f, Alpha);
        NewLocation = FMath::Lerp(NewLocation, FocusTarget, Alpha);

        if (Alpha >= 1.0f)
        {
            bIsFocused = false;
        }
    }
    else
    {
        // WASD移动
        FVector Forward = GetActorForwardVector();
        FVector Right = GetActorRightVector();

        Forward.Z = 0;
        Right.Z = 0;
        Forward.Normalize();
        Right.Normalize();

        float ScaledSpeed = MovementSpeed * (CurrentZoom / MaxZoom);
        NewLocation += Forward * SmoothedInput.Y * ScaledSpeed * DeltaTime;
        NewLocation += Right * SmoothedInput.X * ScaledSpeed * DeltaTime;
    }

    NewLocation = ClampCameraLocation(NewLocation);
    SetActorLocation(NewLocation);
}

void ABioRTSCamera::MoveForward(float Value)
{
    CurrentInput.Y = Value;
}

void ABioRTSCamera::MoveRight(float Value)
{
    CurrentInput.X = Value;
}

void ABioRTSCamera::ZoomIn()
{
    ApplyZoom(-ZoomSpeed);
}

void ABioRTSCamera::ZoomOut()
{
    ApplyZoom(ZoomSpeed);
}

void ABioRTSCamera::ProcessEdgeScrolling(float DeltaTime)
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    float MouseX, MouseY;
    if (!PC->GetMousePosition(MouseX, MouseY)) return;

    int32 ViewportX, ViewportY;
    PC->GetViewportSize(ViewportX, ViewportY);

    FVector Forward = GetActorForwardVector();
    FVector Right = GetActorRightVector();
    Forward.Z = 0; Right.Z = 0;
    Forward.Normalize(); Right.Normalize();

    float ScaledSpeed = EdgeScrollSpeed * (CurrentZoom / MaxZoom) * DeltaTime;

    // 上边缘
    if (MouseY <= EdgeScrollThreshold)
    {
        SetActorLocation(ClampCameraLocation(GetActorLocation() + Forward * ScaledSpeed));
    }
    // 下边缘
    if (MouseY >= ViewportY - EdgeScrollThreshold)
    {
        SetActorLocation(ClampCameraLocation(GetActorLocation() - Forward * ScaledSpeed));
    }
    // 左边缘
    if (MouseX <= EdgeScrollThreshold)
    {
        SetActorLocation(ClampCameraLocation(GetActorLocation() - Right * ScaledSpeed));
    }
    // 右边缘
    if (MouseX >= ViewportX - EdgeScrollThreshold)
    {
        SetActorLocation(ClampCameraLocation(GetActorLocation() + Right * ScaledSpeed));
    }
}

void ABioRTSCamera::FocusOnLocation(FVector Location, float Duration)
{
    FocusTarget = Location;
    FocusTarget.Z = CurrentZoom;
    FocusDuration = FMath::Max(Duration, 0.1f);
    FocusElapsed = 0.0f;
    bIsFocused = true;
}

void ABioRTSCamera::FocusOnActor(AActor* Target, float Duration)
{
    if (Target)
    {
        FocusOnLocation(Target->GetActorLocation(), Duration);
    }
}

void ABioRTSCamera::SetCameraBounds(FVector MinBounds, FVector MaxBounds)
{
    CameraMinBounds = MinBounds;
    CameraMaxBounds = MaxBounds;
}

FVector ABioRTSCamera::ClampCameraLocation(const FVector& Location) const
{
    FVector Clamped = Location;
    Clamped.X = FMath::Clamp(Clamped.X, CameraMinBounds.X, CameraMaxBounds.X);
    Clamped.Y = FMath::Clamp(Clamped.Y, CameraMinBounds.Y, CameraMaxBounds.Y);
    Clamped.Z = FMath::Clamp(Clamped.Z, CameraMinBounds.Z, CameraMaxBounds.Z);
    return Clamped;
}

void ABioRTSCamera::ApplyZoom(float Delta)
{
    CurrentZoom = FMath::Clamp(CurrentZoom + Delta * GetWorld()->GetDeltaSeconds(),
        MinZoom, MaxZoom);

    FVector NewLocation = GetActorLocation();
    NewLocation.Z = CurrentZoom;
    SetActorLocation(NewLocation);
}
