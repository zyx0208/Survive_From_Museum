// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageBox_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "WeaponDataTableBeta.h"
#include "AGSDCharacter.h"

void UStorageBox_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseStorageBoxButton)
    {
        CloseStorageBoxButton->OnClicked.AddDynamic(this, &UStorageBox_UI::CloseStorageBox);
    }
    if (ImageSlot1Button)
    {
        ImageSlot1Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot1Clicked);
    }

    if (ImageSlot2Button)
    {
        ImageSlot2Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot2Clicked);
    }

    // 데이터 테이블에서 무기 메쉬 가져오기
    if (WeaponDataTableBeta)
    {
        static const FString ContextString(TEXT("Weapon Data Context"));
        TArray<FName> RowNames = WeaponDataTableBeta->GetRowNames();

        if (RowNames.Num() > 0)
        {
            FWeaponDataTableBetaStruct* WeaponData1 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowNames[0], ContextString, true);
            if (WeaponData1 && WeaponData1->WeaponIcon)
            {
                DisplayWeaponImage(1, WeaponData1->WeaponIcon);
            }

            if (RowNames.Num() > 1)
            {
                FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowNames[1], ContextString, true);
                if (WeaponData2 && WeaponData2->WeaponIcon)
                {
                    DisplayWeaponImage(2, WeaponData2->WeaponIcon);
                }
            }
        }
    }
}

void UStorageBox_UI::DisplayWeaponImage(int32 SlotIndex, UTexture2D* WeaponIcon)
{
    if (!WeaponIcon)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponIcon is NULL for Slot %d"), SlotIndex);
        return;
    }

    if (SlotIndex == 1 && ImageSlot1)
    {
        ImageSlot1->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 2 && ImageSlot2)
    {
        ImageSlot2->SetBrushFromTexture(WeaponIcon);
    }
}

void UStorageBox_UI::CloseStorageBox()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UStorageBox_UI::OnImageSlot1Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 1 selected."));
}

void UStorageBox_UI::OnImageSlot2Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 2 selected."));
}
