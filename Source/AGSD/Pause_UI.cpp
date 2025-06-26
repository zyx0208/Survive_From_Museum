// Fill out your copyright notice in the Description page of Project Settings.

#include "Pause_UI.h"
#include "AGSDCharacter.h"
#include "Components/WrapBox.h"
#include "Components/Button.h"
#include "AccessoryData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "DrawingBook_UI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

void UPause_UI::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (AccessoryWrapBox && PlayerCharacter && AccessoryDataTable && AccessoryImageClass)
    {
        PopulateAccessoryIcons();
    }
    if (DrawingBoxButton)
    {
        DrawingBoxButton->OnClicked.RemoveDynamic(this, &UPause_UI::OpenDrawingBook);
        DrawingBoxButton->OnClicked.AddDynamic(this, &UPause_UI::OpenDrawingBook);
    }

    if (HPText)
    {
        FString HPString = FString::Printf(TEXT("체력 : %d / %d"), PlayerCharacter->CurrentHealth, PlayerCharacter->MaxHealth);
        HPText->SetText(FText::FromString(HPString));
    }
    if (DefenseText)
    {
        FString DefenseString = FString::Printf(TEXT("방어력 : %.0f%%"), PlayerCharacter->Defense);
        DefenseText->SetText(FText::FromString(DefenseString));
    }
    if (AttackText)
    {
        FString AttackString = FString::Printf(TEXT("공격력 : %.0f"), PlayerCharacter->Attack);
        AttackText->SetText(FText::FromString(AttackString));
    }
    if (AttackSpeedText)
    {
        FString ASString = FString::Printf(TEXT("공격속도 : %.1f"), PlayerCharacter->AttackSpeedLevel);
        AttackSpeedText->SetText(FText::FromString(ASString));
    }
    if (AttackRangeText)
    {
        FString ARString = FString::Printf(TEXT("공격범위 : %.0f"), PlayerCharacter->AttackRangeLevel);
        AttackRangeText->SetText(FText::FromString(ARString));
    }
    if (SpeedText)
    {
        FString SpeedString = FString::Printf(TEXT("이동속도 : %.1f"), PlayerCharacter->SpeedLevel);
        SpeedText->SetText(FText::FromString(SpeedString));
    }
    if (DashText)
    {
        FString DashString = FString::Printf(TEXT("대쉬 쿨타임 : %.0f초"), PlayerCharacter->DashCooldown);
        DashText->SetText(FText::FromString(DashString));
    }
    if (BounsXPText)
    {
        FString BounsXPString = FString::Printf(TEXT("경험치 획득 보너스 : %.1f배"), PlayerCharacter->BounsXPLevel);
        BounsXPText->SetText(FText::FromString(BounsXPString));
    }
    if (XPRangeText)
    {
        FString XPRString = FString::Printf(TEXT("경험치 획득 범위 : %.1f"), PlayerCharacter->XPRangeLevel);
        XPRangeText->SetText(FText::FromString(XPRString));
    }
}

void UPause_UI::PopulateAccessoryIcons()
{
    AccessoryWrapBox->ClearChildren();

    for (const FName& RowName : PlayerCharacter->GetAccessory)
    {
        const FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("PauseUI"));

        if (Accessory && Accessory->AccessoryIcon)
        {
            UUserWidget* IconWidget = CreateWidget<UUserWidget>(GetWorld(), AccessoryImageClass);
            if (IconWidget)
            {
                UFunction* InitFunc = IconWidget->FindFunction(FName("InitializeAccessoryIcon"));
                if (InitFunc)
                {
                    struct FDynamicAccessoryParam
                    {
                        FAccessoryData AccessoryData;
                    };

                    FDynamicAccessoryParam Param;
                    Param.AccessoryData = *Accessory;
                    IconWidget->ProcessEvent(InitFunc, &Param);
                }
                AccessoryWrapBox->AddChildToWrapBox(IconWidget);
            }
        }
    }
}

void UPause_UI::OpenDrawingBook()
{
    if (!DrawingBookClass) return;

    if (DrawingBookWidget && DrawingBookWidget->IsInViewport())
    {
        // 이미 열려있으면 중복 생성 방지
        return;
    }

    // 기존 Pause UI 제거
    RemoveFromParent();

    // DrawingBook UI 생성 및 추가
    DrawingBookWidget = CreateWidget<UDrawingBook_UI>(GetWorld(), DrawingBookClass);
    if (DrawingBookWidget)
    {
        DrawingBookWidget->PauseWidgetRef = this; // 닫기 후 돌아올 수 있도록 참조 전달
        DrawingBookWidget->AddToViewport();
    }
}


