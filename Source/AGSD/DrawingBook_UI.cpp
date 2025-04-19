// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBook_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "AccessoryData.h"
#include "AGSDCharacter.h"

void UDrawingBook_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseDrawingBookButton)
    {
        CloseDrawingBookButton->OnClicked.AddDynamic(this, &UDrawingBook_UI::CloseDrawingBook);
    }
}

void UDrawingBook_UI::CloseDrawingBook()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}