// Fill out your copyright notice in the Description page of Project Settings.


#include "AccessoryIcon_UI.h"
#include "AccessoryDetail_UI.h"
#include "AGSDCharacter.h"
#include "DrawingBook_UI.h"
#include "Kismet/GameplayStatics.h"

void UAccessoryIcon_UI::Init(FAccessoryData* InData)
{
    if (!InData) return;
    AccessoryData = *InData;

    bool bIsKnown = false;
    AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (PlayerCharacter)
    {
        bIsKnown = PlayerCharacter->KnowRowName.Contains(AccessoryData.RowName);
    }

    if (IconImage)
    {
        if (bIsKnown)
        {
            if (AccessoryData.AccessoryIcon)
            {
                IconImage->SetBrushFromTexture(AccessoryData.AccessoryIcon);
            }
        }
        else
        {
            // UnknowImage가 설정되어 있다면 비공개 이미지로 대체
            if (UnknowImage)
            {
                IconImage->SetBrushFromTexture(UnknowImage);
            }
        }
    }
    // 버튼 활성화 여부
    if (IconButton)
    {
        if (bIsKnown)
        {
            IconButton->SetIsEnabled(true);
            IconButton->OnClicked.AddDynamic(this, &UAccessoryIcon_UI::OnIconClicked);
        }
        else
        {
            IconButton->SetIsEnabled(false);  // 비활성화
        }
    }

    // 효과 이미지 초기화: 모두 숨기기
    if (CommonEffect) CommonEffect->SetVisibility(ESlateVisibility::Hidden);
    if (RareEffect) RareEffect->SetVisibility(ESlateVisibility::Hidden);
    if (LegendaryEffect) LegendaryEffect->SetVisibility(ESlateVisibility::Hidden);

    if (bIsKnown)
    {
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