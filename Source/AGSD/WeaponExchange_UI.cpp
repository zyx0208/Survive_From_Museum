// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponExchange_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "WeaponDataTableBeta.h"
#include "AGSDCharacter.h"

void UWeaponExchange_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (WeaponExchangeAgree) {

    }
}

void UWeaponExchange_UI::DisplayWeaponImage(int32 SlotIndex, UTexture2D* WeaponIcon)
{
}

void UWeaponExchange_UI::CloseWeaponExchange()
{
}

void UWeaponExchange_UI::OnImageSlot1Clicked()
{
}

void UWeaponExchange_UI::OnImageSlot2Clicked()
{
}

void UWeaponExchange_UI::OnImageSlotClicked(int32 ButtonIndex)
{
}

void UWeaponExchange_UI::OnAgreeButtonClicked()
{
}

void UWeaponExchange_UI::OnDisagreeButtonClicked()
{
}
