// Fill out your copyright notice in the Description page of Project Settings.


#include "AccessoryIcon_UI.h"
#include "AccessoryDetail_UI.h"
#include "DrawingBook_UI.h"

void UAccessoryIcon_UI::Init(FAccessoryData* InData)
{
    if (!InData) return;
    AccessoryData = *InData;
    if (IconImage && AccessoryData.AccessoryIcon)
    {
        IconImage->SetBrushFromTexture(AccessoryData.AccessoryIcon);
    }
    if (IconButton)
    {
        IconButton->OnClicked.AddDynamic(this, &UAccessoryIcon_UI::OnIconClicked);
    }

    // 효과 이미지 초기화: 모두 숨기기
    if (CommonEffect) CommonEffect->SetVisibility(ESlateVisibility::Hidden);
    if (RareEffect) RareEffect->SetVisibility(ESlateVisibility::Hidden);
    if (LegendaryEffect) LegendaryEffect->SetVisibility(ESlateVisibility::Hidden);

    // Enum 기반으로 등급에 맞는 이미지 표시
    switch (AccessoryData.Rarity)
    {
    case EAccessoryRarity::Common:
        if (CommonEffect)
        {
            CommonEffect->SetVisibility(ESlateVisibility::Visible);
        }
        break;

    case EAccessoryRarity::Rare:
        if (RareEffect)
        {
            RareEffect->SetVisibility(ESlateVisibility::Visible);
        }
        break;

    case EAccessoryRarity::Legendary:
        if (LegendaryEffect)
        {
            LegendaryEffect->SetVisibility(ESlateVisibility::Visible);
        }
        break;

    default:
        break;
    }
}

void UAccessoryIcon_UI::OnIconClicked()
{
    // Detail 창이 이미 켜져 있으면 무시
    if (DrawingBookRef && DrawingBookRef->IsDetailOpen())
    {
        return;
    }
    UAccessoryDetail_UI* Detail = CreateWidget<UAccessoryDetail_UI>(GetWorld(), AccessoryDetailClass);
    if (Detail)
    {
        Detail->SetAccessory(AccessoryData);
        Detail->SetDrawingBookRef(DrawingBookRef);
        Detail->AddToViewport(100);

        // DrawingBook에 등록 (닫을 수 있게)
        if (DrawingBookRef)
        {
            DrawingBookRef->SetDetailWidget(Detail);
        }
    }
}