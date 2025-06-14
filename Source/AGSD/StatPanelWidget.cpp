// Fill out your copyright notice in the Description page of Project Settings.


#include "StatPanelWidget.h"
#include "Components/VerticalBox.h"
#include "StatBarWidget.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UStatPanelWidget::SetupStatBars(AAGSDCharacter* Character)
{
    if (!Character) return;

    if (StatBar_Attack)
        StatBar_Attack->InitializeStat(FText::FromString("Attack"), Character->Attack, 0.f, 1.f);

    if (StatBar_Defense)
        StatBar_Defense->InitializeStat(FText::FromString("Defense"), Character->Defense, 0.f, 10.f);

    if (StatBar_AS)
        StatBar_AS->InitializeStat(FText::FromString("AS"), Character->AttackSpeedLevel, 1.f, 0.1f);

    if (StatBar_AR)
        StatBar_AR->InitializeStat(FText::FromString("AR"), Character->AttackRangeLevel, 0.f, 1.f);

    if (StatBar_Speed)
        StatBar_Speed->InitializeStat(FText::FromString("Speed"), Character->SpeedLevel, 500.f, 50.f);
}
