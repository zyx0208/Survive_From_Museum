// PauseUI_AccessoryIcon.cpp

#include "PauseUI_AccessoryIcon.h"
#include "PauseUI_AccessoryDetail.h"
#include "Components/Image.h"
#include "Layout/Geometry.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UPauseUI_AccessoryIcon::InitializeAccessoryIcon(const FAccessoryData& InData)
{
    AccessoryData = InData;

    if (AccessoryIcon && InData.AccessoryIcon)
    {
        AccessoryIcon->SetBrushFromTexture(InData.AccessoryIcon);
    }
}

void UPauseUI_AccessoryIcon::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
    ShowDetailUI(InGeometry);
}

void UPauseUI_AccessoryIcon::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);
    HideDetailUI();
}

void UPauseUI_AccessoryIcon::ShowDetailUI_Implementation(const FGeometry& InGeometry)
{
    if (!DetailBalloonClass || SpawnedBalloon) return;

    if (UWorld* World = GetWorld())
    {
        SpawnedBalloon = CreateWidget<UPauseUI_AccessoryDetail>(World, DetailBalloonClass);
        if (SpawnedBalloon)
        {
            SpawnedBalloon->SetAccessoryDetail(AccessoryData);
            SpawnedBalloon->AddToViewport();

            FVector2D WidgetScreenPos = InGeometry.GetAbsolutePosition() / UWidgetLayoutLibrary::GetViewportScale(this);
            FVector2D Offset(-150.0f, 0.0f);
            SpawnedBalloon->SetPositionInViewport(WidgetScreenPos + Offset, false);
        }
    }
}

void UPauseUI_AccessoryIcon::HideDetailUI_Implementation()
{
    if (SpawnedBalloon && SpawnedBalloon->IsInViewport())
    {
        SpawnedBalloon->RemoveFromParent();
    }
    SpawnedBalloon = nullptr;
}
