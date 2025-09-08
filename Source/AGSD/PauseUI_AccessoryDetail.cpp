// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUI_AccessoryDetail.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UPauseUI_AccessoryDetail::NativeConstruct()
{
    Super::NativeConstruct();

    // 커서 기준 중앙 정렬(원하면 0,0으로 바꿔 좌상단 기준)
    SetAlignmentInViewport(ViewportAlignment);

    // 생성 시점에 한 번 위치 동기화
    if (APlayerController* PC = GetOwningPlayer())
    {
        float MX, MY;
        if (UWidgetLayoutLibrary::GetMousePositionScaledByDPI(PC, MX, MY))
        {
            SetAlignmentInViewport(ViewportAlignment);
            SetPositionInViewport(FVector2D(MX, MY) + FollowOffset, true);
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
            // 기본 위치(마우스 + 오프셋)
            FVector2D Pos(MX, MY);
            Pos += FollowOffset;

            // 화면 가장자리 밖으로 나가지 않게 클램프
            const FVector2D ViewSize = UWidgetLayoutLibrary::GetViewportSize(this);

            // 현재 위젯의 실제 크기(첫 틱엔 0일 수 있어 보정)
            FVector2D WidgetSize = MyGeometry.GetLocalSize();
            if (WidgetSize.X <= KINDA_SMALL_NUMBER || WidgetSize.Y <= KINDA_SMALL_NUMBER)
            {
                WidgetSize = GetDesiredSize();
            }

            // Alignment를 고려한 중앙(또는 지정 정렬) 기준 클램프 범위 계산
            const FVector2D MinPos = FVector2D(EdgePadding + WidgetSize.X * ViewportAlignment.X,
                EdgePadding + WidgetSize.Y * ViewportAlignment.Y);
            const FVector2D MaxPos = FVector2D(ViewSize.X - EdgePadding - WidgetSize.X * (1.f - ViewportAlignment.X),
                ViewSize.Y - EdgePadding - WidgetSize.Y * (1.f - ViewportAlignment.Y));

            Pos.X = FMath::Clamp(Pos.X, MinPos.X, MaxPos.X);
            Pos.Y = FMath::Clamp(Pos.Y, MinPos.Y, MaxPos.Y);

            // DPI 보정된 좌표를 넣으므로 true 사용
            SetPositionInViewport(FVector2D(MX, MY) + FollowOffset, true);
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
