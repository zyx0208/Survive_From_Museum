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

    if (WeaponExchangeAgreeButton) {
        WeaponExchangeAgreeButton->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnAgreeButtonClicked);
    }

    if (WeaponExchangedisagreeButton) {
        WeaponExchangedisagreeButton->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnDisagreeButtonClicked);
    }

    if (ImageSlot1Button) {
        ImageSlot1Button->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnImageSlot1Clicked);
    }

    if (ImageSlot2Button) {
        ImageSlot2Button->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnImageSlot2Clicked);
    }

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

            RowName = FName(PlayerCharacter->OverlapID);
            WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIconNewWeapon = WeaponData->WeaponIcon;

            DisplayWeaponImage(WeaponIcon1, WeaponIcon2, WeaponIconNewWeapon);
        }
    }
}

void UWeaponExchange_UI::DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2, UTexture2D* WeaponIconNewWeapon)
{
    if (ImageSlot1) {
        ImageSlot1->SetBrushFromTexture(WeaponIcon1);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (ImageSlot2) {
        ImageSlot2->SetBrushFromTexture(WeaponIcon2);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (ImageSlotNewWeapon) {
        ImageSlotNewWeapon->SetBrushFromTexture(WeaponIconNewWeapon);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }
}

void UWeaponExchange_UI::CloseWeaponExchange()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UWeaponExchange_UI::OnImageSlot1Clicked()
{
    if (HighlightedButtons.Contains(0)) {
        return;
    }

    OnImageSlotClicked(0);
}

void UWeaponExchange_UI::OnImageSlot2Clicked()
{
    if (HighlightedButtons.Contains(1)) {
        return;
    }

    OnImageSlotClicked(1);
}

void UWeaponExchange_UI::OnImageSlotClicked(int32 ButtonIndex)
{
    HighlightedButtons.Reset();
    HighlightedButtons.Add(ButtonIndex);

    FLinearColor NewColor = ImageSlot1Button->GetBackgroundColor();
    NewColor.A = 1.0f;
    ImageSlot1Button->SetBackgroundColor(NewColor);
    ImageSlot2Button->SetBackgroundColor(NewColor);

    if (HighlightedButtons.Contains(0)){
        NewColor.A = 0.0f;
        ImageSlot1Button->SetBackgroundColor(NewColor);
    }
    if (HighlightedButtons.Contains(1)) {
        NewColor.A = 0.0f;
        ImageSlot2Button->SetBackgroundColor(NewColor);
    }
}

void UWeaponExchange_UI::OnAgreeButtonClicked()
{
    if (HighlightedButtons.Num() == 0) {
        return;
    }

    //HighlightedButtons[0]

    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        FString Overlap = PlayerCharacter->OverlapID;
        PlayerCharacter->WeaponArray[HighlightedButtons[0]] = FCString::Atoi(*Overlap);
        CloseWeaponExchange();
        PlayerCharacter->WeaponTake();
    }
    UE_LOG(LogTemp, Display, TEXT("AgreeButtonClicked"));
}

void UWeaponExchange_UI::OnDisagreeButtonClicked()
{
    CloseWeaponExchange();
    UE_LOG(LogTemp, Display, TEXT("DisAgreeButtonClicked"));
}
