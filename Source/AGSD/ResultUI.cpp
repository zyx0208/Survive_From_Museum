// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUI.h"
#include "AGSDGameMode.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UResultUI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseResultButton)
    {
        CloseResultButton->OnClicked.AddDynamic(this, &UResultUI::CloseResult);
    }
    if (WeaponDataTable) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
        {
            FName RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTable->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* Icon1 = WeaponData->WeaponIcon;

            RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            WeaponData = WeaponDataTable->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* Icon2 = WeaponData->WeaponIcon;

            DisplayWeaponImage(Icon1, Icon2);
        }
    }
    PopulateAccessoryIcons();
}

void UResultUI::CloseResult()
{
    AAGSDGameMode* Gamemode = GetWorld() ? Cast<AAGSDGameMode>(GetWorld()->GetAuthGameMode<AAGSDGameMode>()) : nullptr;
    if (Gamemode)
    {
        //Gamemode->ResetAccessoryData();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode 찾을 수 없음"));
    }
}

void UResultUI::DisplayWeaponImage(UTexture2D* Icon1, UTexture2D* Icon2)
{
    if (WeaponIcon1) {
        WeaponIcon1->SetBrushFromTexture(Icon1);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (WeaponIcon2) {
        WeaponIcon2->SetBrushFromTexture(Icon2);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot2"));
    }
}

void UResultUI::PopulateAccessoryIcons()
{
    if (!AccessoryWrapBox && !AccessoryDataTable) return;
    AccessoryWrapBox->ClearChildren();

    static const FString ContextString(TEXT("PopulateAccessotyIcons"));
    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();

    AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
    if (!PlayerCharacter) return;

    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, ContextString, true);

        if (Accessory && PlayerCharacter->AcquiredAccessories.Contains(RowName) && Accessory->AccessoryIcon)
        {
            // 새로운 UImage 위젯 생성
            UImage* NewAccessoryImage = NewObject<UImage>(this);
            if (NewAccessoryImage)
            {
                NewAccessoryImage->SetBrushFromTexture(Accessory->AccessoryIcon);
                // 브러쉬의 이미지 사이즈를 수정하여 아이콘 크기를 변경합니다.
                FSlateBrush Brush = NewAccessoryImage->Brush;
                Brush.ImageSize = FVector2D(120.0f, 120.0f);
                NewAccessoryImage->SetBrush(Brush);

                NewAccessoryImage->SetVisibility(ESlateVisibility::Visible);

                // WrapBox에 추가
                AccessoryWrapBox->AddChildToWrapBox(NewAccessoryImage);
                //UE_LOG(LogTemp, Log, TEXT("액세서리 추가: %s"), *RowName.ToString());
            }
        }
    }
}