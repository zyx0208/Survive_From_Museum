// Fill out your copyright notice in the Description page of Project Settings.


#include "StatBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

void UStatBarWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UStatBarWidget::InitializeStat(FText InStatName, float InValue, float InStartOffset, float InUnitValue)
{
    StatName = InStatName;
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
    }

    UpdateBarDisplay();
}

void UStatBarWidget::UpdateBarDisplay()
{
    if (!BarContainer || !BarClass) return;

    UE_LOG(LogTemp, Warning, TEXT("UI생성: %s %.1f %.1f %.1f"),
        *StatName.ToString(),
        CurrentValue,
        StartOffset,
        UnitValue);


    BarContainer->ClearChildren();

    float Delta = CurrentValue - StartOffset;
    int32 FullBars = FMath::FloorToInt(Delta / UnitValue);
    float Remainder = FMath::Fmod(Delta, UnitValue) / UnitValue;

    // 색상 결정
    FLinearColor FillColor = FLinearColor::White;
    FString StatKey = StatName.ToString();

    if (StatKey == TEXT("Attack"))
        FillColor = FLinearColor::Red;
    else if (StatKey == TEXT("Defense"))
        FillColor = FLinearColor::Blue;
    else if (StatKey == TEXT("AS"))
        FillColor = FLinearColor::Yellow;
    else if (StatKey == TEXT("AR"))
        FillColor = FLinearColor::Green;
    else if (StatKey == TEXT("Speed"))
        FillColor = FLinearColor(0.5f, 0.8f, 1.0f);

    for (int32 i = 0; i < MaxBars; ++i)
    {
        UUserWidget* BarWidget = CreateWidget<UUserWidget>(this, BarClass);
        if (!BarWidget) continue;

        UProgressBar* ProgressBar = Cast<UProgressBar>(BarWidget->GetWidgetFromName(TEXT("UnitProgressBar")));
        if (ProgressBar)
        {
            float Fill = 0.f;
            if (i < FullBars) Fill = 1.f;
            else if (i == FullBars) Fill = Remainder;

            ProgressBar->SetPercent(Fill);
            ProgressBar->SetFillColorAndOpacity(FillColor);
        }

        BarContainer->AddChildToHorizontalBox(BarWidget);
    }
}


void UStatBarWidget::PlayBlinkEffect()
{
    // 애니메이션 연동: Blueprint에서 Animation 생성 후 PlayAnimation 호출
    //PlayAnimationByName(TEXT("Blink"), 0.f, 1);
}