// Fill out your copyright notice in the Description page of Project Settings.


#include "DrawingBook_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "AccessoryData.h"
#include "AGSDCharacter.h"
#include "AccessoryIcon_UI.h"
#include "Pause_UI.h"
#include "MainLoby_UI.h"
#include "AccessoryDetail_UI.h"

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
    /*if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }*/
    RemoveFromParent(); // 현재 UI 제거
   

    // Pause UI 복원
    if (PauseWidgetRef)
    {
        PauseWidgetRef->AddToViewport();       
        PauseWidgetRef->DrawingBookWidget = nullptr; // 재사용 방지를 위해 포인터 초기화
    }
    if (MainWidgetRef)
    {
        MainWidgetRef->AddToViewport();
        MainWidgetRef->DrawingBookWidget = nullptr;
    }
}

void UDrawingBook_UI::PopulateAccessoryIcons()
{
    if (!AccessoryWrapBox || !AccessoryDataTable) return;

    AccessoryWrapBox->ClearChildren();

    // 모든 Row 수집
    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();
    TArray<FAccessoryData*> AccessoryList;
    AccessoryList.Reserve(RowNames.Num());
    for (const FName& RowName : RowNames)
    {
        if (FAccessoryData* Row = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("DrawingBook Populate")))
        {
            AccessoryList.Add(Row);
        }
    }

    // 정렬: 테마 > 등급 > RowName
    AccessoryList.Sort([this](const FAccessoryData& A, const FAccessoryData& B)
        {
            const int32 ATheme = GetThemePriority(A.Theme);
            const int32 BTheme = GetThemePriority(B.Theme);
            if (ATheme != BTheme) return ATheme < BTheme;

            const int32 ARarity = GetRarityPriority(A.Rarity);
            const int32 BRarity = GetRarityPriority(B.Rarity);
            if (ARarity != BRarity) return ARarity < BRarity;

            return A.RowName.ToString() < B.RowName.ToString();
        });

    // 아이콘 생성
    for (FAccessoryData* Data : AccessoryList)
    {
        if (!Data) continue;

        UAccessoryIcon_UI* IconWidget = CreateWidget<UAccessoryIcon_UI>(this, AccessoryIconClass);
        if (IconWidget)
        {
            IconWidget->Init(Data);
            // 아이콘 클릭 시 이 DrawingBook에 세부정보 표시
            IconWidget->SetDrawingBookRef(this);
            AccessoryWrapBox->AddChild(IconWidget);
        }
    }
}

int32 UDrawingBook_UI::GetThemePriority(const FString& Theme)
{
    if (Theme == TEXT("")) return 0;
    if (Theme == TEXT("Elyon")) return 1;
    if (Theme == TEXT("JK")) return 2;
    if (Theme == TEXT("Togetherium")) return 3;
    if (Theme == TEXT("SteelRoid")) return 4;
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

void UDrawingBook_UI::ShowAccessoryDetail(const FAccessoryData& InData)
{
    // 아이콘
    if (AccessoryIconImage)
    {
        if (InData.AccessoryIcon)
        {
            FSlateBrush Brush;
            Brush.SetResourceObject(InData.AccessoryIcon);
            // 원본 사이즈 반영(선택)
            FVector2D Size = FVector2D(
                InData.AccessoryIcon->GetSizeX(),
                InData.AccessoryIcon->GetSizeY());
            Brush.ImageSize = Size;

            AccessoryIconImage->SetBrush(Brush);
        }
        else
        {
            AccessoryIconImage->SetBrushFromTexture(nullptr);
        }
    }

    // 이름
    if (AccessoryNameText)
    {
        AccessoryNameText->SetText(FText::FromString(InData.AccessoryName));
    }

    // 효과: “/” → 줄바꿈
    if (AccessoryEffectText)
    {
        const FString Replaced = InData.AccessoryEffect.Replace(TEXT("/"), TEXT("\n"));
        AccessoryEffectText->SetText(FText::FromString(Replaced));
    }

    // 설명
    if (AccessoryDescriptionText)
    {
        AccessoryDescriptionText->SetText(FText::FromString(InData.AccessoryDescription));
    }
}