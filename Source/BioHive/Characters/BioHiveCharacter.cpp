// BioHiveCharacter.cpp - 第一人称生物母巢核心角色实现
#include "BioHiveCharacter.h"
#include "UI/BioHiveHUD.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/InputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

ABioHiveCharacter::ABioHiveCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 创建第一人称相机
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetMesh(), TEXT("Head"));
    FirstPersonCamera->bUsePawnControlRotation = true;

    // 创建吞噬检测范围
    ConsumeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ConsumeSphere"));
    ConsumeSphere->SetupAttachment(RootComponent);
    ConsumeSphere->SetSphereRadius(ConsumeRange);

    // 默认器官成本
    OrganCosts = { 10.0f, 25.0f, 50.0f, 100.0f, 200.0f };
}

void ABioHiveCharacter::BeginPlay()
{
    Super::BeginPlay();

    // 获取HUD引用
    APlayerController* PC = GetController<APlayerController>();
    if (PC)
    {
        BioHiveHUD = Cast<ABioHiveHUD>(PC->GetHUD());
        UpdateHUD();
    }
}

void ABioHiveCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABioHiveCharacter::SetupPlayerInputComponent()
{
    Super::SetupPlayerInputComponent();

    UInputComponent* Input = GetInputComponent();
    if (!Input) return;

    Input->BindAxis("MoveForward", this, &ABioHiveCharacter::MoveForward);
    Input->BindAxis("MoveRight", this, &ABioHiveCharacter::MoveRight);
    Input->BindAxis("LookUp", this, &ABioHiveCharacter::LookUp);
    Input->BindAxis("Turn", this, &ABioHiveCharacter::Turn);
    Input->BindAction("Consume", IE_Pressed, this, &ABioHiveCharacter::TryConsume);
    Input->BindAction("EvolveOrgan1", IE_Pressed, this, &ABioHiveCharacter::EvolveOrganWithUI).ExecuteIfBound(0);
    Input->BindAction("EvolveOrgan2", IE_Pressed, this, &ABioHiveCharacter::EvolveOrganWithUI).ExecuteIfBound(1);
    Input->BindAction("EvolveOrgan3", IE_Pressed, this, &ABioHiveCharacter::EvolveOrganWithUI).ExecuteIfBound(2);
    Input->BindAction("EvolveOrgan4", IE_Pressed, this, &ABioHiveCharacter::EvolveOrganWithUI).ExecuteIfBound(3);
    Input->BindAction("EvolveOrgan5", IE_Pressed, this, &ABioHiveCharacter::EvolveOrganWithUI).ExecuteIfBound(4);
}

void ABioHiveCharacter::MoveForward(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorForwardVector() * Value);
    }
}

void ABioHiveCharacter::MoveRight(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(GetActorRightVector() * Value);
    }
}

void ABioHiveCharacter::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void ABioHiveCharacter::Turn(float Value)
{
    AddControllerYawInput(Value);
}

void ABioHiveCharacter::TryConsume()
{
    // 检查冷却
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastConsumeTime < ConsumeCooldown)
    {
        return;
    }

    // 射线检测前方物体
    FVector StartLocation = FirstPersonCamera->GetComponentLocation();
    FVector EndLocation = StartLocation + (FirstPersonCamera->GetForwardVector() * ConsumeRange);

    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::LineTraceSingle(
        GetWorld(),
        StartLocation,
        EndLocation,
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        TArray<AActor*>(),
        EDrawDebugTrace::None,
        HitResult,
        true
    );

    if (bHit && HitResult.GetActor())
    {
        ConsumeTarget(HitResult.GetActor());
        LastConsumeTime = CurrentTime;
    }
}

void ABioHiveCharacter::ConsumeTarget(AActor* Target)
{
    if (!Target) return;

    // 简单的生物质获取逻辑
    float BiomassGain = 10.0f; // 基础值

    // 根据目标类型调整获取量
    FString TargetName = Target->GetName();
    if (TargetName.Contains("Small"))
    {
        BiomassGain = 5.0f;
    }
    else if (TargetName.Contains("Large"))
    {
        BiomassGain = 25.0f;
    }

    Biomass += BiomassGain;

    // 销毁被吞噬的目标
    Target->Destroy();

    // 更新UI
    UpdateHUD();

    // 输出调试信息
    UE_LOG(LogTemp, Log, TEXT("Consumed target! Gained %.1f biomass. Total: %.1f"),
           BiomassGain, Biomass);
}

bool ABioHiveCharacter::EvolveOrgan(int32 OrganIndex)
{
    // 检查索引有效性
    if (OrganIndex < 0 || OrganIndex >= OrganCosts.Num())
    {
        return false;
    }

    // 检查是否已解锁
    int32 OrganBit = 1 << OrganIndex;
    if (UnlockedOrgans & OrganBit)
    {
        return false; // 已经解锁
    }

    // 检查是否有足够的生物质
    float Cost = OrganCosts[OrganIndex];
    if (Biomass < Cost)
    {
        return false; // 生物质不足
    }

    // 消耗生物质并解锁器官
    Biomass -= Cost;
    UnlockedOrgans |= OrganBit;

    UE_LOG(LogTemp, Log, TEXT("Evolved organ %d! Remaining biomass: %.1f"),
           OrganIndex, Biomass);

    return true;
}

bool ABioHiveCharacter::EvolveOrganWithUI(int32 OrganIndex)
{
    bool bSuccess = EvolveOrgan(OrganIndex);
    if (bSuccess)
    {
        UpdateHUD();
    }
    return bSuccess;
}

void ABioHiveCharacter::UpdateHUD()
{
    if (BioHiveHUD)
    {
        BioHiveHUD->SetBiomassDisplay(Biomass);
        BioHiveHUD->SetOrgansDisplay(UnlockedOrgans);
    }
}
