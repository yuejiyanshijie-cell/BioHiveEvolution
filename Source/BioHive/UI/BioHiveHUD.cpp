// BioHiveHUD.cpp - 简单HUD实现
#include "BioHiveHUD.h"
#include "Engine/Canvas.h"

ABioHiveHUD::ABioHiveHUD()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABioHiveHUD::BeginPlay()
{
    Super::BeginPlay();
}

void ABioHiveHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!Canvas) return;

    // 设置字体和颜色
    FCanvasTextItem TextItem(FVector2D(20, 20), FText::GetEmpty(), HUDFont, FLinearColor::White);
    TextItem.FontSize = FontSize;
    TextItem.EnableShadow(FLinearColor::Black);

    // 显示生物质
    FString BiomassText = FString::Printf(TEXT("Biomass: %.1f"), DisplayBiomass);
    TextItem.Text = FText::FromString(BiomassText);
    TextItem.Position = FVector2D(20, 20);
    Canvas->DrawItem(TextItem);

    // 显示器官数量
    FString OrgansText = FString::Printf(TEXT("Organs Unlocked: %d"), DisplayOrgans);
    TextItem.Text = FText::FromString(OrgansText);
    TextItem.Position = FVector2D(20, 50);
    Canvas->DrawItem(TextItem);

    // 显示操作提示
    FString HelpText = TEXT("WASD: Move | Mouse: Look | E: Consume | 1-5: Evolve Organ");
    TextItem.Text = FText::FromString(HelpText);
    TextItem.Position = FVector2D(20, Canvas->ClipY - 40);
    TextItem.FontSize = 16;
    Canvas->DrawItem(TextItem);
}

void ABioHiveHUD::SetBiomassDisplay(float NewBiomass)
{
    DisplayBiomass = NewBiomass;
}

void ABioHiveHUD::SetOrgansDisplay(int32 Organs)
{
    DisplayOrgans = Organs;
}
