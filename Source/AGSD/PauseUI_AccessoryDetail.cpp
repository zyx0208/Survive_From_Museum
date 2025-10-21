// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUI_AccessoryDetail.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UPauseUI_AccessoryDetail::NativeConstruct()
{
    Super::NativeConstruct();

    // 커서 기준 중앙 정렬(원하면 0,0으로 바꿔 좌상단 기준)
    SetAlignmentInViewport(ViewportAlignment);

    //SetAnchorsInViewport(PreSetAnchors);

    // 생성 시점에 한 번 위치 동기화
    if (APlayerController* PC = GetOwningPlayer())
    {
        float MX, MY;
        if (UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, MX, MY))
        {
            // 위젯 크기 확보 (첫 프레임 보정)
            FVector2D WidgetSize = GetDesiredSize();
            if (WidgetSize.X <= KINDA_SMALL_NUMBER || WidgetSize.Y <= KINDA_SMALL_NUMBER)
            {
                // Construct 직후엔 DesiredSize가 0일 수 있음 → 대략값 보정 (필요 없다면 생략)
                WidgetSize = FVector2D(300.f, 160.f);
            }

            
            FVector2D Pos(MX, MY);
            Pos -= (WidgetSize * ViewportAlignment);
            Pos += FollowOffset;

            SetPositionInViewport(Pos, /*bRemoveDPIScale=*/true);
        }
    }
}
void UPauseUI_AccessoryDetail::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!bFollowMouse) return;

    if (APlayerController* PC = GetOwningPlayer())
    {
        float MX, MY;
        if (UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, MX, MY))
        {
            const FVector2D ViewSize = UWidgetLayoutLibrary::GetViewportSize(this);

            FVector2D WidgetSize = MyGeometry.GetLocalSize();
            if (WidgetSize.X <= KINDA_SMALL_NUMBER || WidgetSize.Y <= KINDA_SMALL_NUMBER)
            {
                WidgetSize = GetDesiredSize();
                if (WidgetSize.X <= KINDA_SMALL_NUMBER || WidgetSize.Y <= KINDA_SMALL_NUMBER)
                {
                    // 여전히 0이면 임시 보정값
                    WidgetSize = FVector2D(300.f, 160.f);
                }
            }

            FVector2D Pos(MX, MY);
            Pos -= (WidgetSize * ViewportAlignment);  // 정렬 보정
            Pos += FollowOffset;

            const FVector2D MinPos = FVector2D(EdgePadding, EdgePadding);
            const FVector2D MaxPos = FVector2D(
                ViewSize.X - EdgePadding - WidgetSize.X,
                ViewSize.Y - EdgePadding - WidgetSize.Y
            );

            Pos.X = FMath::Clamp(Pos.X, MinPos.X, MaxPos.X);
            Pos.Y = FMath::Clamp(Pos.Y, MinPos.Y, MaxPos.Y);

            SetPositionInViewport(Pos, /*bRemoveDPIScale=*/true);
        }
    }
}

void UPauseUI_AccessoryDetail::SetAccessoryDetail(const FAccessoryData& InData)
{
    if (NameText)
    {
        NameText->SetText(FText::FromString(InData.AccessoryName));
    }
    if (EffectText)
    {
        FString ReplacedEffect = InData.AccessoryEffect.Replace(TEXT("/"), TEXT("\n"));
        ReplacedEffect = ReplacedEffect.Replace(TEXT(" "), TEXT(""));
        EffectText->SetText(FText::FromString(ReplacedEffect));
    }
    if (DescriptionText)
    {
        DescriptionText->SetText(FText::FromString(InData.AccessoryDescription));
    }
}
