// Fill out your copyright notice in the Description page of Project Settings.


#include "TrapTimerWidget.h"
#include "Components/TextBlock.h"

void UTrapTimerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 초기 표시(선택)
    if (TimerText)
    {
        TimerText->SetText(FText::FromString(TEXT("")));
    }
}

void UTrapTimerWidget::SetSeconds(int32 Seconds)
{
    if (!TimerText) return;

    // 숫자만 표시
    TimerText->SetText(FText::AsNumber(FMath::Max(0, Seconds)));
}
