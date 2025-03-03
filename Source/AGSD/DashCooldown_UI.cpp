// Fill out your copyright notice in the Description page of Project Settings.


#include "DashCooldown_UI.h"
#include "Components/Image.h"
#include "AGSDCharacter.h"
#include "WeaponDataTableBeta.h"
#include "Components/ProgressBar.h"

void UDashCooldown_UI::UpdateWeaponIcon()
{
    if (WeaponDataTableBeta) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
        {
            FName RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIcon1 = WeaponData->WeaponIcon;     

            RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIcon2 = WeaponData->WeaponIcon;

            if (MainWeaponIcon && SubWeaponIcon && mainicon) 
            {
                MainWeaponIcon->SetBrushFromTexture(WeaponIcon1);
                SubWeaponIcon->SetBrushFromTexture(WeaponIcon2);
            }
            else
            {
                MainWeaponIcon->SetBrushFromTexture(WeaponIcon2);
                SubWeaponIcon->SetBrushFromTexture(WeaponIcon1);
            }
        }
    }
}
void UDashCooldown_UI::UpdateSwapWeapon()
{
    if (mainicon)
    {
        UpdateWeaponIcon();
        mainicon = false;
    }
    else
    {
        UpdateWeaponIcon();
        mainicon = true;
    }
}

void UDashCooldown_UI::UpdateDashCooldown(float CooldownPercentage)
{
    if (DashCooldownBar)
    {
        DashCooldownBar->SetPercent(CooldownPercentage);
    }
}

