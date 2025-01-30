// Fill out your copyright notice in the Description page of Project Settings.


#include "DashCooldown_UI.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UDashCooldown_UI::UpdateDashCooldown(float CooldownPercentage)
{
    if (DashCooldownBar)
    {
        DashCooldownBar->SetPercent(CooldownPercentage);
    }
}

