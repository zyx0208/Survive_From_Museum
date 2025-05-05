// Fill out your copyright notice in the Description page of Project Settings.


#include "Damage_UI.h"

void UDamage_UI::NativeConstruct()
{
    DamageText->SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Log, TEXT("DamageUICreated"))
}
void UDamage_UI::DamageTextCreate(float damage, FVector2D screenPosition)
{
    UTextBlock* NewDamage = NewObject<UTextBlock>(this, FName("Damage"));
    if (NewDamage) {
        UE_LOG(LogTemp, Log, TEXT("DamageCreated"));
        FWidgetTransform NewTransform;
        NewTransform.Translation = screenPosition;
        NewDamage->SetRenderTransform(NewTransform);
        NewDamage->SetVisibility(ESlateVisibility::Visible);
    }
}
