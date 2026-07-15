// BioHivePlayerController.cpp - RTS玩家控制器实现

#include "BioHivePlayerController.h"
#include "BioHiveGameMode.h"
#include "Camera/BioRTSCamera.h"
#include "Units/BioUnitSelectionManager.h"
#include "Units/BioUnit.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ABioHivePlayerController::ABioHivePlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void ABioHivePlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 添加增强输入映射
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        if (BioHiveMappingContext)
        {
            Subsystem->AddMappingContext(BioHiveMappingContext, 0);
        }
    }

    // 切换到RTS相机
    SetViewTargetWithBlend(RTSCamera, 0.5f);
}

void ABioHivePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput) return;

    EnhancedInput->BindAction(IA_MoveForward, ETriggerEvent::Triggered,
        this, &ABioHivePlayerController::Input_MoveForward);
    EnhancedInput->BindAction(IA_MoveRight, ETriggerEvent::Triggered,
        this, &ABioHivePlayerController::Input_MoveRight);

    EnhancedInput->BindAction(IA_ToggleView, ETriggerEvent::Started,
        this, &ABioHivePlayerController::ToggleViewMode);

    EnhancedInput->BindAction(IA_SelectClick, ETriggerEvent::Started,
        this, &ABioHivePlayerController::OnClickPressed);
    EnhancedInput->BindAction(IA_SelectClick, ETriggerEvent::Completed,
        this, &ABioHivePlayerController::OnClickReleased);

    EnhancedInput->BindAction(IA_CommandClick, ETriggerEvent::Started,
        this, &ABioHivePlayerController::OnRightClick);

    EnhancedInput->BindAction(IA_BuildMenu, ETriggerEvent::Started,
        this, &ABioHivePlayerController::Input_ToggleBuildMenu);
}

void ABioHivePlayerController::ToggleViewMode()
{
    if (bIsRTSMode && FirstPersonUnit.IsValid())
    {
        // 切换到第一人称
        SetViewTargetWithBlend(FirstPersonUnit.Get(), 0.3f);
        bIsRTSMode = false;
        FirstPersonUnit->EnterFirstPersonView();
    }
    else
    {
        // 切换回RTS
        ExitFirstPersonMode();
    }
}

void ABioHivePlayerController::EnterFirstPersonMode(ABioUnit* Unit)
{
    if (!Unit) return;

    FirstPersonUnit = Unit;
    if (bIsRTSMode)
    {
        ToggleViewMode();
    }
}

void ABioHivePlayerController::ExitFirstPersonMode()
{
    if (!bIsRTSMode && RTSCamera)
    {
        SetViewTargetWithBlend(RTSCamera, 0.3f);
        bIsRTSMode = true;

        if (FirstPersonUnit.IsValid())
        {
            FirstPersonUnit->ExitFirstPersonView();
        }
    }
}

void ABioHivePlayerController::Input_MoveForward(const FInputActionValue& Value)
{
    if (RTSCamera)
    {
        RTSCamera->MoveForward(Value.Get<float>());
    }
}

void ABioHivePlayerController::Input_MoveRight(const FInputActionValue& Value)
{
    if (RTSCamera)
    {
        RTSCamera->MoveRight(Value.Get<float>());
    }
}

void ABioHivePlayerController::Input_ZoomIn()
{
    if (RTSCamera)
    {
        RTSCamera->ZoomIn();
    }
}

void ABioHivePlayerController::Input_ZoomOut()
{
    if (RTSCamera)
    {
        RTSCamera->ZoomOut();
    }
}

void ABioHivePlayerController::Input_ToggleBuildMenu()
{
    // 蓝图事件处理，切换建造菜单UI
}

void ABioHivePlayerController::Input_SelectAll()
{
    ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (ABioUnitSelectionManager* SM = GM->GetSelectionManager())
    {
        SM->SelectAllUnits();
    }
}

void ABioHivePlayerController::Input_HoldPosition()
{
    ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (ABioUnitSelectionManager* SM = GM->GetSelectionManager())
    {
        SM->IssueHoldPositionCommand();
    }
}

void ABioHivePlayerController::OnClickPressed()
{
    if (!bIsRTSMode) return;

    // 开始框选
    GetMousePosition(SelectionStart.X, SelectionStart.Y);
    bIsSelecting = true;
}

void ABioHivePlayerController::OnClickReleased()
{
    if (!bIsRTSMode) return;

    GetMousePosition(SelectionEnd.X, SelectionEnd.Y);

    if (bIsSelecting)
    {
        float DragDistance = FVector2D::Distance(SelectionStart, SelectionEnd);

        if (DragDistance > 10.0f)
        {
            // 框选
            PerformSelection();
        }
        else
        {
            // 点选
            FHitResult Hit;
            GetHitResultUnderCursor(ECC_Visibility, false, Hit);

            ABioUnitSelectionManager* SM = nullptr;
            if (ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode()))
            {
                SM = GM->GetSelectionManager();
            }

            if (Hit.GetActor() && SM)
            {
                ABioUnit* Unit = Cast<ABioUnit>(Hit.GetActor());
                if (Unit)
                {
                    bool bAddToSelection = IsInputKeyDown(EKeys::LeftControl);
                    SM->SelectUnit(Unit, bAddToSelection);
                }
                else
                {
                    SM->DeselectAll();
                }
            }
            else if (SM)
            {
                SM->DeselectAll();
            }
        }
    }

    bIsSelecting = false;
}

void ABioHivePlayerController::OnRightClick()
{
    if (!bIsRTSMode) return;

    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    ABioUnitSelectionManager* SM = GM->GetSelectionManager();
    if (!SM) return;

    SM->IssueSmartCommand(Hit.GetActor(), Hit.Location);
}

void ABioHivePlayerController::PerformSelection()
{
    ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (ABioUnitSelectionManager* SM = GM->GetSelectionManager())
    {
        bool bAddToSelection = IsInputKeyDown(EKeys::LeftControl);
        SM->SelectUnitsInRectangle(SelectionStart, SelectionEnd, bAddToSelection);
    }
}

void ABioHivePlayerController::Input_Formation(int32 Key)
{
    ABioHiveGameMode* GM = Cast<ABioHiveGameMode>(GetWorld()->GetAuthGameMode());
    if (!GM) return;

    if (ABioUnitSelectionManager* SM = GM->GetSelectionManager())
    {
        if (IsInputKeyDown(EKeys::LeftControl))
        {
            // Ctrl+数字键创建编队
            SM->CreateFormation(Key);
        }
        else
        {
            // 数字键选择编队
            SM->SelectFormation(Key);
        }
    }
}
