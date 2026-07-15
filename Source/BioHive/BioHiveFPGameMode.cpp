// BioHiveFPGameMode.cpp - 第一人称测试游戏模式实现
#include "BioHiveFPGameMode.h"
#include "Characters/BioHiveCharacter.h"
#include "UI/BioHiveHUD.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"

ABioHiveFPGameMode::ABioHiveFPGameMode()
{
    // 设置默认角色类
    DefaultPawnClass = ABioHiveCharacter::StaticClass();

    // 设置HUD类
    HUDClass = ABioHiveHUD::StaticClass();
}

void ABioHiveFPGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Log, TEXT("BioHive First Person Test Mode Started"));
}
