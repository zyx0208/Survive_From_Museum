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

    TArray<UButton*> ImageSlotButtons = { ImageSlot1Button, ImageSlot2Button, ImageSlot3Button, ImageSlot4Button,
                                          ImageSlot5Button, ImageSlot6Button, ImageSlot7Button, ImageSlot8Button };

    // #무기업그레이드
    WeaponIndexSetArray = { 4, 5, 9, 7, 10, 6, 8, 1 }; //무기 기초 배열(IID값)
    UpgradeWeaponIndexSetArray = { 0, 0, 11, 0, 0, 0, 0, 0 };   //무기 업그레이드시 IID값 변경 설정

    UpdateWeaponIndexSetArray();

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

        for (int32 i = 0; i < 8; i++)
        {
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowNames[WeaponIndexSetArray[i]], ContextString, true);
            // bIsAcquired 값이 false면 해당 버튼을 숨김
            if (!WeaponData->bIsAcquired && ImageSlotButtons.IsValidIndex(i))
            {
                ImageSlotButtons[i]->SetVisibility(ESlateVisibility::Hidden);
                DisplayWeaponImage(i + 1, RockIcon);
            }
            else if (WeaponData->WeaponIcon) // 무기 아이콘 표시
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
        if (HighlightedButtons.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("CloseStorageBox: No buttons selected. Keeping current weapons."));
        }
        else
        {
            // 최소 하나라도 선택된 경우 변경 적용
            if (HighlightedButtons.IsValidIndex(0))
            {
                PlayerCharacter->WeaponArray[0] = HighlightedButtons[0];
            }
            if (HighlightedButtons.IsValidIndex(1))
            {
                PlayerCharacter->WeaponArray[1] = HighlightedButtons[1];
            }

            UE_LOG(LogTemp, Log, TEXT("CloseStorageBox: Weapon selection applied."));
        }
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
    UImage* ImageSlots[8] = {
        ImageSlot1, ImageSlot2, ImageSlot3, ImageSlot4,
        ImageSlot5, ImageSlot6, ImageSlot7, ImageSlot8
    };

    if (!ImageSlots[ButtonIndex]) return;

    // 클릭된 이미지의 텍스처 가져오기
    UTexture2D* SelectedTexture = Cast<UTexture2D>(ImageSlots[ButtonIndex]->Brush.GetResourceObject());
    if (!SelectedTexture) return;

    if (HighlightedButtons.Contains(WeaponIndexSetArray[ButtonIndex]))
    {
        // 선택 해제
        HighlightedButtons.Remove(WeaponIndexSetArray[ButtonIndex]);

        if (SelectedSlotIndex1 == WeaponIndexSetArray[ButtonIndex])
        {
            SelectedImageSlot1->SetBrushFromTexture(nullptr);
            SelectedSlotIndex1 = -1;
        }
        else if (SelectedSlotIndex2 == WeaponIndexSetArray[ButtonIndex])
        {
            SelectedImageSlot2->SetBrushFromTexture(nullptr);
            SelectedSlotIndex2 = -1;
        }
    }
    else
    {
        // 두 개 이상 선택 불가 처리
        if (HighlightedButtons.Num() >= 2)
        {
            int32 OldestIndex = HighlightedButtons[0];
            HighlightedButtons.RemoveAt(0);

            if (SelectedSlotIndex1 == OldestIndex)
            {
                SelectedImageSlot1->SetBrushFromTexture(nullptr);
                SelectedSlotIndex1 = -1;
            }
            else if (SelectedSlotIndex2 == OldestIndex)
            {
                SelectedImageSlot2->SetBrushFromTexture(nullptr);
                SelectedSlotIndex2 = -1;
            }
        }

        HighlightedButtons.Add(WeaponIndexSetArray[ButtonIndex]);

        if (SelectedSlotIndex1 == -1)
        {
            SelectedImageSlot1->SetBrushFromTexture(SelectedTexture);
            SelectedSlotIndex1 = WeaponIndexSetArray[ButtonIndex];
        }
        else if (SelectedSlotIndex2 == -1)
        {
            SelectedImageSlot2->SetBrushFromTexture(SelectedTexture);
            SelectedSlotIndex2 = WeaponIndexSetArray[ButtonIndex];
        }
    }
}

void UStorageBox_UI::UpdateWeaponIndexSetArray()
{
    // 데이터 테이블에서 무기 메쉬 가져오기
    if (!WeaponDataTableBeta) return;
    static const FString ContextString(TEXT("UpdateWeaponIndexSetArray"));
    // 데이터 테이블의 모든 행 이름을 가져옵니다.
    TArray<FName> RowNames = WeaponDataTableBeta->GetRowNames();
    // WeaponIndexSetArray의 각 요소에 대해 확인
    for (int32 i = 0; i < WeaponIndexSetArray.Num(); i++)
    {
        int32 CurrentIID = WeaponIndexSetArray[i];
        // 데이터 테이블의 각 행을 순회하면서 IID가 일치하는지 확인합니다.
        for (const FName& RowName : RowNames)
        {
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            if (WeaponData && WeaponData->IID == CurrentIID)
            {
                // bIsReinforce가 true인 경우 해당 인덱스를 10으로 변경합니다.
                if (WeaponData->bIsReinforce)
                {
                    WeaponIndexSetArray[i] = UpgradeWeaponIndexSetArray[i];
                    UE_LOG(LogTemp, Log, TEXT("WeaponIndexSetArray[%d] with IID %d is reinforced, updated to %d."), i, CurrentIID, UpgradeWeaponIndexSetArray[i]);
                }
            }
        }
    }
}