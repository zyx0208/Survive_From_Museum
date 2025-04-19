// Fill out your copyright notice in the Description page of Project Settings.


#include "AccessoryIcon_UI.h"
#include "AccessoryDetail_UI.h"

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
    
}

void UAccessoryIcon_UI::OnIconClicked()
{
    UAccessoryDetail_UI* Detail = CreateWidget<UAccessoryDetail_UI>(GetWorld(), AccessoryDetailClass);
    if (Detail)
    {
        Detail->SetAccessory(AccessoryData);
        Detail->AddToViewport(100);
    }
}