// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUI_AccessoryDetail.h"
#include "Components/TextBlock.h"

void UPauseUI_AccessoryDetail::SetAccessoryDetail(const FAccessoryData& InData)
{
    if (NameText)
    {
        NameText->SetText(FText::FromString(InData.AccessoryName));
    }
    if (EffectText)
    {
        FString ReplacedEffect = InData.AccessoryEffect.Replace(TEXT("/"), TEXT("\n"));
        EffectText->SetText(FText::FromString(ReplacedEffect));
    }
    if (DescriptionText)
    {
        DescriptionText->SetText(FText::FromString(InData.AccessoryDescription));
    }
}
