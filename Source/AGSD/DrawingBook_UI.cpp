// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBook_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "AccessoryData.h"
#include "AGSDCharacter.h"
#include "AccessoryIcon_UI.h"

void UDrawingBook_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseDrawingBookButton)
    {
        CloseDrawingBookButton->OnClicked.AddDynamic(this, &UDrawingBook_UI::CloseDrawingBook);
    }
    PopulateAccessoryIcons();
}

void UDrawingBook_UI::CloseDrawingBook()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UDrawingBook_UI::PopulateAccessoryIcons()
{
    if (!AccessoryWrapBox && !AccessoryDataTable) return;
    AccessoryWrapBox->ClearChildren();
    // 모든 Row 가져오기
    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();
    TArray<FAccessoryData*> AccessoryList;
    for (const FName& RowName : RowNames)
    {
        if (FAccessoryData* Row = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("")))
        {
            AccessoryList.Add(Row);
        }
    }
    // 정렬: 테마 > 등급 > RowName
    AccessoryList.Sort([this](const FAccessoryData& A, const FAccessoryData& B)
        {
            int32 ATheme = GetThemePriority(A.Theme);
            int32 BTheme = GetThemePriority(B.Theme);
            if (ATheme != BTheme) return ATheme < BTheme;

            int32 ARarity = GetRarityPriority(A.Rarity);
            int32 BRarity = GetRarityPriority(B.Rarity);
            if (ARarity != BRarity) return ARarity < BRarity;

            return A.RowName.ToString() < B.RowName.ToString();
        });

    // 버튼 생성 및 WrapBox에 추가
    for (FAccessoryData* Data : AccessoryList)
    {
        if (!Data) continue;

        UAccessoryIcon_UI* IconWidget = CreateWidget<UAccessoryIcon_UI>(this, AccessoryIconClass); // 클래스 지정 필요
        if (IconWidget)
        {
            IconWidget->Init(Data); 
            AccessoryWrapBox->AddChild(IconWidget);
        }
    }
}

int32 UDrawingBook_UI::GetThemePriority(const FString& Theme)
{
    if (Theme == TEXT("")) return 0;
    if (Theme == TEXT("Jelda")) return 1;
    if (Theme == TEXT("Mario")) return 2;
    if (Theme == TEXT("will")) return 3;
    if (Theme == TEXT("gal")) return 4;
    return 999;
}

int32 UDrawingBook_UI::GetRarityPriority(EAccessoryRarity Rarity)
{
    switch (Rarity)
    {
    case EAccessoryRarity::Legendary: return 0;
    case EAccessoryRarity::Rare: return 1;
    case EAccessoryRarity::Common: return 2;
    default: return 999;
    }
}