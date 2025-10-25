// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUI_AccessoryDetail.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "TimerManager.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UPauseUI_AccessoryDetail::NativeConstruct()
{
    Super::NativeConstruct();

    // 디테일창이 마우스 히트테스트를 가로채지 않도록
    SetVisibility(ESlateVisibility::HitTestInvisible);

    SetAnchorsInViewport(FAnchors(0.f, 0.f, 0.f, 0.f));
    SetAlignmentInViewport(FVector2D::ZeroVector);

    SetRenderOpacity(0.f);

    UpdatePositionToMouse(/*bClampToViewport=*/true);

    SetRenderOpacity(1.f);

    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
    {
        ViewportResizedHandle = GEngine->GameViewport->Viewport->ViewportResizedEvent.AddUObject(
            this, &UPauseUI_AccessoryDetail::HandleViewportResized
        );
    }
}
void UPauseUI_AccessoryDetail::NativeDestruct()
{
    if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport && ViewportResizedHandle.IsValid())
    {
        GEngine->GameViewport->Viewport->ViewportResizedEvent.Remove(ViewportResizedHandle);
    }
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(ReapplyNextTickHandle);
    }
    Super::NativeDestruct();
}

void UPauseUI_AccessoryDetail::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    if (!bFollowMouse) return;
    
    UpdatePositionToMouse(true, &MyGeometry);
}


FVector2D UPauseUI_AccessoryDetail::ComputeWidgetSize(const FGeometry* GeometryPtr) const
{
    if (ContentRoot)
    {
        const FVector2D S = ContentRoot->GetDesiredSize();
        if (!S.IsNearlyZero()) return S;
    }
    if (GeometryPtr)
    {
        const FVector2D S = GeometryPtr->GetLocalSize();
        if (!S.IsNearlyZero()) return S;
    }
    const FVector2D Desired = GetDesiredSize();
    return !Desired.IsNearlyZero() ? Desired : FVector2D(300.f, 160.f);
}

void UPauseUI_AccessoryDetail::UpdatePositionToMouse(bool bClampToViewport, const FGeometry* GeometryPtr)
{
    // 1) 마우스 절대(스크린) 좌표
    const FVector2D CursorAbs = FSlateApplication::Get().GetCursorPos();

    // 2) 뷰포트 위젯 지오메트리 (DPI/윈도우 스케일/전체화면 변화 포함한 실제 좌표계)
    const FGeometry ViewportGeo = UWidgetLayoutLibrary::GetViewportWidgetGeometry(this);

    // 3) 절대 → 뷰포트 로컬좌표로 변환
    FVector2D MouseLocal = ViewportGeo.AbsoluteToLocal(CursorAbs);

    // 4) 정렬(좌상단) + 오프셋
    //    (Alignment은 SetAlignmentInViewport(0,0)로 유지 중이므로 사이즈 보정 불필요)
    //    FollowOffset도 동일한 로컬좌표 단위로 동작
    FVector2D PosLocal = MouseLocal + FollowOffset;

    // 5) 클램프도 뷰포트 로컬 크기 기준
    if (bClampToViewport)
    {
        const FVector2D ViewLocalSize = ViewportGeo.GetLocalSize();

        // 위젯 실제 크기 (UMG/Slate 단위) — 지오메트리/DesiredSize 그대로 사용
        const FVector2D WidgetSize = ComputeWidgetSize(GeometryPtr);

        const FVector2D MinPos(EdgePadding, EdgePadding);
        const FVector2D MaxPos(
            ViewLocalSize.X - EdgePadding - WidgetSize.X,
            ViewLocalSize.Y - EdgePadding - WidgetSize.Y
        );

        PosLocal.X = FMath::Clamp(PosLocal.X, MinPos.X, MaxPos.X);
        PosLocal.Y = FMath::Clamp(PosLocal.Y, MinPos.Y, MaxPos.Y);
    }

    // 6) 뷰포트 "로컬좌표"이므로 항상 false
    SetPositionInViewport(PosLocal, /*bRemoveDPIScale=*/false);
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

void UPauseUI_AccessoryDetail::HandleViewportResized(FViewport* /*Viewport*/, uint32 /*Unused*/)
{
    // 전환 직후 한 번 즉시 재적용
    ReapplyLayoutNow();
    // 그리고 다음 틱에도 한 번 더(엔진 DPI/레이아웃 안정화 보정)
    ReapplyLayoutNextTick();
}

void UPauseUI_AccessoryDetail::ReapplyLayoutNow()
{
    InvalidateLayoutAndVolatility();
    if (TSharedPtr<SWidget> Slate = TakeWidget()) { Slate->SlatePrepass(); }

    UpdatePositionToMouse(/*bClampToViewport=*/true);
}


void UPauseUI_AccessoryDetail::ReapplyLayoutNextTick()
{
    if (UWorld* W = GetWorld())
    {
        W->GetTimerManager().ClearTimer(ReapplyNextTickHandle);
        W->GetTimerManager().SetTimer(
            ReapplyNextTickHandle,
            FTimerDelegate::CreateUObject(this, &UPauseUI_AccessoryDetail::ReapplyLayoutNow),
            0.f, false
        );
    }
}

bool UPauseUI_AccessoryDetail::GetMouseViewportPositionPx(FVector2D& OutPosPx) const
{
    const FVector2D ScreenPos = FSlateApplication::Get().GetCursorPos();

    FVector2D ViewportPosPx;
    USlateBlueprintLibrary::ScreenToViewport(GetWorld(), ScreenPos, ViewportPosPx);
    OutPosPx = ViewportPosPx;

    const FVector2D ViewSizePx = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
    return (OutPosPx.X >= 0.f && OutPosPx.Y >= 0.f &&
        OutPosPx.X <= ViewSizePx.X && OutPosPx.Y <= ViewSizePx.Y);
}

