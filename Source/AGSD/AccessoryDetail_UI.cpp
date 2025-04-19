// Fill out your copyright notice in the Description page of Project Settings.


#include "AccessoryDetail_UI.h"

void UAccessoryDetail_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseAccessoryDetailButton)
    {
        CloseAccessoryDetailButton->OnClicked.AddDynamic(this, &UAccessoryDetail_UI::CloseAccessoryDetail);
    }
}

void UAccessoryDetail_UI::CloseAccessoryDetail()
{
    RemoveFromParent();
}

void UAccessoryDetail_UI::SetAccessory(const FAccessoryData& InData)
{
    AccessoryData = InData;
    if (AccessoryNameText)
    {
        AccessoryNameText->SetText(FText::FromString(AccessoryData.AccessoryName));
    }

    if (AccessoryEffectText)
    {
        AccessoryEffectText->SetText(FText::FromString(AccessoryData.AccessoryEffect));
    }

    if (AccessoryDescriptionText)
    {
        AccessoryDescriptionText->SetText(FText::FromString(AccessoryData.AccessoryDescription));
    }

    if (AccessoryIconImage && AccessoryData.AccessoryIcon)
    {
        AccessoryIconImage->SetBrushFromTexture(AccessoryData.AccessoryIcon);
    }
}