// Fill out your copyright notice in the Description page of Project Settings.


#include "StatPanelWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "StatBarWidget.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UStatPanelWidget::SetupStatBars(AAGSDCharacter* Character)
{
    if (!Character) return;

    if (StatBar_Attack)
        StatBar_Attack->InitializeStat(FText::FromString("Attack"), Character->Attack, 1.f, 1.f);

    if (StatBar_Defense)
        StatBar_Defense->InitializeStat(FText::FromString("Defense"), Character->Defense, 100.f, 10.f);

    if (StatBar_AS)
        StatBar_AS->InitializeStat(FText::FromString("AS"), Character->AttackSpeedLevel, 1.f, 0.1f);

    if (StatBar_AR)
        StatBar_AR->InitializeStat(FText::FromString("AR"), Character->AttackRangeLevel, 1.f, 1.f);

    if (StatBar_Speed)
        StatBar_Speed->InitializeStat(FText::FromString("Speed"), Character->SpeedLevel, 500.f, 50.f);

    if (StatBar_XPLevel)
        StatBar_XPLevel->InitializeStat(FText::FromString("XPLevel"), Character->BounsXPLevel, 1.f, 0.1f);

    if (StatBar_XPRange)
        StatBar_XPRange->InitializeStat(FText::FromString("XPRange"), Character->XPRangeLevel, 200.f, 20.f);

    if (HPText)
    {
        FString HPString = FString::Printf(TEXT("체력 : %d / %d"), Character->CurrentHealth, Character->MaxHealth);
        HPText->SetText(FText::FromString(HPString));
    }
    if (DefenseText)
    {
        FString DefenseString = FString::Printf(TEXT("방어력 : %.0f%%"), Character->Defense);
        DefenseText->SetText(FText::FromString(DefenseString));
    }
    if (AttackText)
    {
        FString AttackString = FString::Printf(TEXT("공격력 : %.0f"), Character->Attack);
        AttackText->SetText(FText::FromString(AttackString));
    }
    if (AttackSpeedText)
    {
        FString ASString = FString::Printf(TEXT("공격속도 : %.1f"), Character->AttackSpeedLevel);
        AttackSpeedText->SetText(FText::FromString(ASString));
    }
    if (AttackRangeText)
    {
        FString ARString = FString::Printf(TEXT("공격범위 : %.0f"), Character->AttackRangeLevel);
        AttackRangeText->SetText(FText::FromString(ARString));
    }
    if (SpeedText)
    {
        FString SpeedString = FString::Printf(TEXT("이동속도 : %.1f"), Character->SpeedLevel);
        SpeedText->SetText(FText::FromString(SpeedString));
    }
    if (DashText)
    {
        FString DashString = FString::Printf(TEXT("대쉬 쿨타임 : %.0f초"), Character->DashCooldown);
        DashText->SetText(FText::FromString(DashString));
    }
    if (BounsXPText)
    {
        FString BounsXPString = FString::Printf(TEXT("경험치 획득 보너스 : %.1f배"), Character->BounsXPLevel);
        BounsXPText->SetText(FText::FromString(BounsXPString));
    }
    if (XPRangeText)
    {
        FString XPRString = FString::Printf(TEXT("경험치 획득 범위 : %.1f"), Character->XPRangeLevel);
        XPRangeText->SetText(FText::FromString(XPRString));
    }
}

void UStatPanelWidget::ToggleStatBox()
{
    if (!StatListBox2) return;
    if (!StatBar_Attack || !StatBar_Defense || !StatBar_AS || !StatBar_AR || !StatBar_Speed || !StatBar_XPLevel || !StatBar_XPRange) return;
    if (!StatImage1 || !StatImage2 || !StatImage3) return;

    const bool bIsBox1Visible = StatBar_Attack->GetVisibility() == ESlateVisibility::Visible;
    const bool bIsBox2Visible = StatListBox2->GetVisibility() == ESlateVisibility::Visible;

    if (bIsBox1Visible && !bIsBox2Visible) //아이콘 켜진상태
    {
        StatListBox2->SetVisibility(ESlateVisibility::Visible);

        StatBar_Attack->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_Defense->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_AS->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_AR->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_Speed->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_XPLevel->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_XPRange->SetVisibility(ESlateVisibility::Collapsed);

        StatImage1->SetVisibility(ESlateVisibility::Visible);
        StatImage2->SetVisibility(ESlateVisibility::Visible);
        StatImage3->SetVisibility(ESlateVisibility::Visible);
    }
    else if (!bIsBox1Visible && bIsBox2Visible) //상세스탯 켜진상태
    {
        StatListBox2->SetVisibility(ESlateVisibility::Collapsed);

        StatBar_Attack->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_Defense->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_AS->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_AR->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_Speed->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_XPLevel->SetVisibility(ESlateVisibility::Collapsed);
        StatBar_XPRange->SetVisibility(ESlateVisibility::Collapsed);

        StatImage1->SetVisibility(ESlateVisibility::Collapsed);
        StatImage2->SetVisibility(ESlateVisibility::Collapsed);
        StatImage3->SetVisibility(ESlateVisibility::Collapsed);
    }
    else if (!bIsBox1Visible && !bIsBox2Visible) //커진상태
    {
        StatListBox2->SetVisibility(ESlateVisibility::Collapsed);

        StatBar_Attack->SetVisibility(ESlateVisibility::Visible);
        StatBar_Defense->SetVisibility(ESlateVisibility::Visible);
        StatBar_AS->SetVisibility(ESlateVisibility::Visible);
        StatBar_AR->SetVisibility(ESlateVisibility::Visible);
        StatBar_Speed->SetVisibility(ESlateVisibility::Visible);
        StatBar_XPLevel->SetVisibility(ESlateVisibility::Visible);
        StatBar_XPRange->SetVisibility(ESlateVisibility::Visible);

        StatImage1->SetVisibility(ESlateVisibility::Visible);
        StatImage2->SetVisibility(ESlateVisibility::Visible);
        StatImage3->SetVisibility(ESlateVisibility::Visible);
    }
}
