// Fill out your copyright notice in the Description page of Project Settings.


#include "StorageBox_UI.h"
#include "Components/Button.h"
#include "AGSDCharacter.h"

void UStorageBox_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseStorageBoxButton)
    {
        CloseStorageBoxButton->OnClicked.AddDynamic(this, &UStorageBox_UI::CloseStorageBox);
    }
}

void UStorageBox_UI::CloseStorageBox()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}
