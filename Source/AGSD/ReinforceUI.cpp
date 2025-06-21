// Fill out your copyright notice in the Description page of Project Settings.


#include "ReinforceUI.h"
#include "WeaponDataTableBeta.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "AGSDCharacter.h"

void UReinforceUI::NativeConstruct()
{
    if (AgreeButton) {
        AgreeButton->OnClicked.AddDynamic(this, &UReinforceUI::OnAgreeButtonClicked);
    }

    if (WeaponDataTableBeta!=nullptr) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
        {
            
            FName RowName1 = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData1 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName1, ContextString, true);
            //UTexture2D* WeaponIcon1 = WeaponData1->WeaponIcon;

            FName RowName2 = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName2, ContextString, true);

            if (UpgradeCheck(*WeaponData1)) {
                ShowReinforce(*WeaponData1);
                Recheck = true;
            }
            else if (UpgradeCheck(*WeaponData2)) {
                ShowReinforce(*WeaponData2);
            }
            //DisplayWeaponImage(WeaponIcon1, WeaponIcon2);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("NO DataTable"));
        CloseUI();
    }
}

void UReinforceUI::DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2)
{
}

void UReinforceUI::CloseUI()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UReinforceUI::OnAgreeButtonClicked()
{
    if (TargetWeapon != nullptr) {
        TargetWeapon->bIsReinforce = true;
        TargetWeapon->bIsAcquired = true;
        UE_LOG(LogTemp, Warning, TEXT("WeaponReinforce"));
    }
    if (Recheck) {
        Recheck = false;
        AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
        FName RowName2 = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
        FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName2, "Recheck", true);
        if (UpgradeCheck(*WeaponData2)) {
            ShowReinforce(*WeaponData2);
        }
        else {
            CloseUI();
        }
    }
    else
    {
        CloseUI();
    }
}

bool UReinforceUI::UpgradeCheck(FWeaponDataTableBetaStruct WeaponData)
{
    int WeaponUpgradeID = WeaponData.UpgradeID;

    if (WeaponUpgradeID != 0) {
        return true;
    }
    return false;
}

void UReinforceUI::Reinforce(FWeaponDataTableBetaStruct WeaponData)
{
}

void UReinforceUI::ShowReinforce(FWeaponDataTableBetaStruct WeaponData)
{
    UTexture2D* WeaponIcon1 = WeaponData.WeaponIcon;
    ImageSlot1->SetBrushFromTexture(WeaponIcon1);
    FName RowName1 = FName(FString::FromInt(WeaponData.UpgradeID));
    WeaponDescription1->SetText(FText::FromString(WeaponData.WeaponDescription));
    OriginalWeaponID = WeaponData.IID;
    TargetWeaponID = WeaponData.UpgradeID;

    static const FString ContextString(TEXT("Weapon Data Context"));
    TargetWeapon = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(FName(FString::FromInt(WeaponData.IID)), ContextString, true);

    FWeaponDataTableBetaStruct* WeaponData1 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName1, ContextString, true);
    UTexture2D* WeaponIcon2 = WeaponData1->WeaponIcon;
    ImageSlot1->SetBrushFromTexture(WeaponIcon2);
    WeaponDescription2->SetText(FText::FromString(WeaponData1->WeaponDescription));
}
