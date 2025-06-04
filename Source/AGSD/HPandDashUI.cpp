// Fill out your copyright notice in the Description page of Project Settings.


#include "HPandDashUI.h"
#include "Components/ProgressBar.h"
#include "AGSDCharacter.h"

void UHPandDashUI::UpdateSimpleHPBar(float HPper)
{
    if (Simple_HPBar)
    {
        Simple_HPBar->SetPercent(HPper);
    }
}

void UHPandDashUI::UpdateSimpleDashBar(float Dashper)
{
    if (Simple_DashBar)
    {
        Simple_DashBar->SetPercent(Dashper);
    }
}