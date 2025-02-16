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
    if (ImageSlot3Button)
    {
        ImageSlot3Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot3Clicked);
    }
    if (ImageSlot4Button)
    {
        ImageSlot4Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot4Clicked);
    }
    if (ImageSlot5Button)
    {
        ImageSlot5Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot5Clicked);
    }
    if (ImageSlot6Button)
    {
        ImageSlot6Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot6Clicked);
    }
    if (ImageSlot7Button)
    {
        ImageSlot7Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot7Clicked);
    }
    if (ImageSlot8Button)
    {
        ImageSlot8Button->OnClicked.AddDynamic(this, &UStorageBox_UI::OnImageSlot8Clicked);
    }

    // 데이터 테이블에서 무기 메쉬 가져오기
    if (WeaponDataTableBeta)
    {
        static const FString ContextString(TEXT("Weapon Data Context"));
        TArray<FName> RowNames = WeaponDataTableBeta->GetRowNames();

        int32 MaxSlots = FMath::Min(RowNames.Num(), 8); // 최대 8개 슬롯까지 처리

        for (int32 i = 0; i < MaxSlots; i++)
        {
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowNames[i], ContextString, true);
            if (WeaponData && WeaponData->WeaponIcon)
            {
                DisplayWeaponImage(i + 1, WeaponData->WeaponIcon);
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
    else if (SlotIndex == 3 && ImageSlot3)
    {
        ImageSlot3->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 4 && ImageSlot4)
    {
        ImageSlot4->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 5 && ImageSlot5)
    {
        ImageSlot5->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 6 && ImageSlot6)
    {
        ImageSlot6->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 7 && ImageSlot7)
    {
        ImageSlot7->SetBrushFromTexture(WeaponIcon);
    }
    else if (SlotIndex == 8 && ImageSlot8)
    {
        ImageSlot8->SetBrushFromTexture(WeaponIcon);
    }
}

void UStorageBox_UI::CloseStorageBox()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->WeaponArray[0] = HighlightedButtons[0];
        PlayerCharacter->WeaponArray[1] = HighlightedButtons[1];
        PlayerCharacter->WeaponSwap();
        PlayerCharacter->WeaponTake();
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UStorageBox_UI::OnImageSlot1Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 1 selected."));
    OnImageSlotClicked(0);
}

void UStorageBox_UI::OnImageSlot2Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 2 selected."));
    OnImageSlotClicked(1);
}

void UStorageBox_UI::OnImageSlot3Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 3 selected."));
    OnImageSlotClicked(2);
}

void UStorageBox_UI::OnImageSlot4Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 4 selected."));
    OnImageSlotClicked(3);
}

void UStorageBox_UI::OnImageSlot5Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 5 selected."));
    OnImageSlotClicked(4);
}

void UStorageBox_UI::OnImageSlot6Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 6 selected."));
    OnImageSlotClicked(5);
}

void UStorageBox_UI::OnImageSlot7Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 7 selected."));
    OnImageSlotClicked(6);
}

void UStorageBox_UI::OnImageSlot8Clicked()
{
    UE_LOG(LogTemp, Log, TEXT("Image Slot 8 selected."));
    OnImageSlotClicked(7);
}

void UStorageBox_UI::OnImageSlotClicked(int32 ButtonIndex)
{
    UButton* ImageSlotButtons[8] = {
        ImageSlot1Button, ImageSlot2Button, ImageSlot3Button, ImageSlot4Button, ImageSlot5Button, ImageSlot6Button, ImageSlot7Button, ImageSlot8Button
    };

    if (!ImageSlotButtons[ButtonIndex]) return;

    if (HighlightedButtons.Contains(ButtonIndex))
    {
        HighlightedButtons.Remove(ButtonIndex);
    }
    else
    {
        if (HighlightedButtons.Num() >= 2)
        {
            HighlightedButtons.RemoveAt(0);
        }
        HighlightedButtons.Add(ButtonIndex);
    }

    for (int32 i = 0; i < 8; i++)
    {
        if (ImageSlotButtons[i])
        {
            FLinearColor NewColor = ImageSlotButtons[i]->GetBackgroundColor();
            NewColor.A = HighlightedButtons.Contains(i) ? 0.3f : 0.0f;
            ImageSlotButtons[i]->SetBackgroundColor(NewColor);
        }
    }
}