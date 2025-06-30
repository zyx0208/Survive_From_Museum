// Fill out your copyright notice in the Description page of Project Settings.


#include "StatBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Animation/UMGSequencePlayer.h"
#include "StatUnitBar.h"

void UStatBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStatBarWidget::InitializeStat(FText InStatName, float InValue, float InStartOffset, float InUnitValue)
{
    StatName = InStatName;
    if (CurrentValue != NULL)
        LastValue = CurrentValue;
    CurrentValue = InValue;
    StartOffset = InStartOffset;
    UnitValue = InUnitValue;

    // 아이콘 설정
    if (StatIconImage)
    {
        FString StatKey = InStatName.ToString();

        if (StatKey == TEXT("Attack"))
        {
            StatIconImage->SetBrushFromTexture(AttackStatIconTexture);
        }
        else if (StatKey == TEXT("Defense"))
        {
            StatIconImage->SetBrushFromTexture(DefenseStatIconTexture);
        }
        else if (StatKey == TEXT("AS"))
        {
            StatIconImage->SetBrushFromTexture(ASStatIconTexture);
        }
        else if (StatKey == TEXT("AR"))
        {
            StatIconImage->SetBrushFromTexture(ARStatIconTexture);
        }
        else if (StatKey == TEXT("Speed"))
        {
            StatIconImage->SetBrushFromTexture(SpeedStatIconTexture);
        }
        else if (StatKey == TEXT("XPLevel"))
        {
            StatIconImage->SetBrushFromTexture(XPLevelIconTexture);
        }
        else if (StatKey == TEXT("XPRange"))
        {
            StatIconImage->SetBrushFromTexture(XPRangeIconTexture);
        }
    }

    UpdateBarDisplay();
}

void UStatBarWidget::UpdateBarDisplay()
{
    if (!BarContainer || !BarClass) return;

    /*UE_LOG(LogTemp, Warning, TEXT("UI생성: %s %.1f %.1f %.1f"),
        *StatName.ToString(),
        CurrentValue,
        StartOffset,
        UnitValue);*/


    BarContainer->ClearChildren();

    float Delta = CurrentValue - StartOffset;
    int32 FullBars = FMath::FloorToInt(Delta / UnitValue);
    float Remainder = FMath::Abs(FMath::Fmod(Delta, UnitValue)) / UnitValue;

    // 색상 결정
    FLinearColor FillColor = FLinearColor::White;
    FString StatKey = StatName.ToString();
    int32 TotalBars = 0;

    if(Delta >= 0.f)
    { 
        TotalBars = FullBars + (Remainder > 0.f ? 1 : 0); 
        
        if (StatKey == TEXT("Attack"))
            FillColor = FLinearColor::Green;
        else if (StatKey == TEXT("Defense"))
            FillColor = FLinearColor::Green;
        else if (StatKey == TEXT("AS"))
            FillColor = FLinearColor::Green;
        else if (StatKey == TEXT("AR"))
            FillColor = FLinearColor::Green;
        else if (StatKey == TEXT("Speed"))
            FillColor = FLinearColor::Green;
        else if(StatKey == TEXT("XPLevel"))
            FillColor = FLinearColor::Green;
        else if (StatKey == TEXT("XPRange"))
            FillColor = FLinearColor::Green;
    }
    else
    {
        TotalBars = FMath::Abs(FullBars) + (Remainder > 0.f ? 1 : 0);
        FillColor = FLinearColor::Black;
    }
   

    for (int32 i = 0; i < TotalBars; ++i)
    {
        UUserWidget* BarWidget = CreateWidget<UUserWidget>(this, BarClass);
        if (!BarWidget) continue;

        UProgressBar* ProgressBar = Cast<UProgressBar>(BarWidget->GetWidgetFromName(TEXT("UnitProgressBar")));
        if (ProgressBar)
        {
            float Fill = 1.f;
            if (i == FullBars && Remainder > 0.f)
                Fill = Remainder;

            ProgressBar->SetPercent(Fill);
            ProgressBar->SetFillColorAndOpacity(FillColor);
        }

        // 추가: 깜빡이기 효과
        int32 OldBars = FMath::FloorToInt((LastValue - StartOffset) / UnitValue);
        if ((LastValue < CurrentValue && i >= OldBars && i < TotalBars) ||  // 증가
            (LastValue > CurrentValue && i >= TotalBars && i < OldBars))    // 감소
        {
            UStatUnitBar* UnitBar = Cast<UStatUnitBar>(BarWidget);
            if (UnitBar)
            {
                UnitBar->PlayBlink(); // PlayBlink()는 BlueprintImplementableEvent로 선언되어야 함
            }
        }
        BarContainer->AddChildToHorizontalBox(BarWidget);
    }
}


