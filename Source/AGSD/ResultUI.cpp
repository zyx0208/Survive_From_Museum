// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUI.h"
#include "AGSDGameMode.h"
#include "Kismet/GameplayStatics.h"

void UResultUI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseResultButton)
    {
        CloseResultButton->OnClicked.AddDynamic(this, &UResultUI::CloseResult);
    }
    PopulateAccessoryIcons();
}

void UResultUI::CloseResult()
{
    AAGSDGameMode* Gamemode = GetWorld() ? Cast<AAGSDGameMode>(GetWorld()->GetAuthGameMode<AAGSDGameMode>()) : nullptr;
    if (Gamemode)
    {
        Gamemode->ResetAccessoryData();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode 찾을 수 없음"));
    }
}

void UResultUI::PopulateAccessoryIcons()
{
    if (!AccessoryWrapBox && !AccessoryDataTable) return;
    AccessoryWrapBox->ClearChildren();

    static const FString ContextString(TEXT("PopulateAccessotyIcons"));
    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();

    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, ContextString, true);
        if (Accessory && Accessory->bIsAcquired && Accessory->AccessoryIcon)
        {
            // 새로운 UImage 위젯 생성
            UImage* NewAccessoryImage = NewObject<UImage>(this);
            if (NewAccessoryImage)
            {
                NewAccessoryImage->SetBrushFromTexture(Accessory->AccessoryIcon);
                NewAccessoryImage->SetVisibility(ESlateVisibility::Visible);
                NewAccessoryImage->SetDesiredSizeOverride(FVector2D(332, 332)); // 아이콘 크기 설정

                // WrapBox에 추가
                AccessoryWrapBox->AddChildToWrapBox(NewAccessoryImage);
                UE_LOG(LogTemp, Log, TEXT("액세서리 추가: %s"), *RowName.ToString());
            }
        }
    }
}