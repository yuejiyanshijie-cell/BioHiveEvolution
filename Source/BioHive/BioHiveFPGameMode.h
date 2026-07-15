// BioHiveFPGameMode.h - 第一人称测试游戏模式
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BioHiveFPGameMode.generated.h"

UCLASS()
class BIOHIVE_API ABioHiveFPGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ABioHiveFPGameMode();

protected:
    virtual void BeginPlay() override;
};
