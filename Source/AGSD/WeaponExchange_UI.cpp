// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponExchange_UI.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "WeaponDataTableBeta.h"
#include "WeaponDrop.h"
#include "AGSDGameInstance.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UWeaponExchange_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (WeaponExchangeAgreeButton) {
        WeaponExchangeAgreeButton->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnAgreeButtonClicked);
    }

    if (WeaponExchangedisagreeButton) {
        WeaponExchangedisagreeButton->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnDisagreeButtonClicked);
    }

    if (ImageSlot1Button) {
        ImageSlot1Button->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnImageSlot1Clicked);
    }

    if (ImageSlot2Button) {
        ImageSlot2Button->OnClicked.AddDynamic(this, &UWeaponExchange_UI::OnImageSlot2Clicked);
    }

    if (WeaponDataTableBeta) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
        {
            FName RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIcon1 = WeaponData->WeaponIcon;

            RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIcon2 = WeaponData->WeaponIcon;

            RowName = FName(PlayerCharacter->OverlapID);
            WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* WeaponIconNewWeapon = WeaponData->WeaponIcon;
            FString NewWeaponDescription = WeaponData->WeaponDescription;
            FString NewWeaponName = WeaponData->Sname;

            DisplayWeaponImage(WeaponIcon1, WeaponIcon2, WeaponIconNewWeapon, NewWeaponDescription,NewWeaponName);
        }
    }

    EffectImage.Add(EffectImage1);
    EffectImage.Add(EffectImage2);
    EffectImage[0]->SetVisibility(ESlateVisibility::Hidden);
    EffectImage[1]->SetVisibility(ESlateVisibility::Hidden);
}

void UWeaponExchange_UI::DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2, UTexture2D* WeaponIconNewWeapon, FString WeaponDescription, FString WeaponName)
{
    if (ImageSlot1) {
        ImageSlot1->SetBrushFromTexture(WeaponIcon1);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (ImageSlot2) {
        ImageSlot2->SetBrushFromTexture(WeaponIcon2);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (ImageSlotNewWeapon) {
        ImageSlotNewWeapon->SetBrushFromTexture(WeaponIconNewWeapon);
        WeaponDescriptionTextBlock->SetText(FText::FromString(WeaponDescription));
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (WeaponNameTextBlock) {
        WeaponNameTextBlock->SetText(FText::FromString(WeaponName));
    }
}

void UWeaponExchange_UI::CloseWeaponExchange()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}

void UWeaponExchange_UI::OnImageSlot1Clicked()
{
    if (HighlightedButtons.Contains(0)) {
        return;
    }

    OnImageSlotClicked(0);
}

void UWeaponExchange_UI::OnImageSlot2Clicked()
{
    if (HighlightedButtons.Contains(1)) {
        return;
    }

    OnImageSlotClicked(1);
}

void UWeaponExchange_UI::OnImageSlotClicked(int32 ButtonIndex)
{
    HighlightedButtons.Reset();
    HighlightedButtons.Add(ButtonIndex);
    EffectImage[0]->SetVisibility(ESlateVisibility::Hidden);
    EffectImage[1]->SetVisibility(ESlateVisibility::Hidden);
    FLinearColor NewColor = ImageSlot1Button->GetBackgroundColor();
    NewColor.A = 1.0f;
    ImageSlot1Button->SetBackgroundColor(NewColor);
    ImageSlot2Button->SetBackgroundColor(NewColor);

    if (HighlightedButtons.Contains(0)){
        NewColor.A = 0.0f;
        ImageSlot1Button->SetBackgroundColor(NewColor);
        EffectImage[0]->SetVisibility(ESlateVisibility::Visible);
        EffectImage[1]->SetVisibility(ESlateVisibility::Hidden);
    }
    if (HighlightedButtons.Contains(1)) {
        NewColor.A = 0.0f;
        ImageSlot2Button->SetBackgroundColor(NewColor);
        EffectImage[0]->SetVisibility(ESlateVisibility::Hidden);
        EffectImage[1]->SetVisibility(ESlateVisibility::Visible);
    }
}

void UWeaponExchange_UI::OnAgreeButtonClicked()
{
    if (HighlightedButtons.Num() == 0) {
        return;
    }

    //HighlightedButtons[0]

    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        AWeaponDrop* OverlapWeapon = PlayerCharacter->OverlapWeaponDrop;
        //int32 OverlapID = FCString::Atoi(*PlayerCharacter->OverlapID);
        int32 OverlapID = FCString::Atoi(*OverlapWeapon->WeaponID);
            
        static const FString ContextString(TEXT("OnAgreeButtonClicked"));
        if (!PlayerCharacter->WeaponDataTableRef) return;

        // 현재 맵 이름 (PIE 접두어 제거됨)
        const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, /*bRemovePrefixString=*/true);
        const bool bIsTutorialMap = CurrentLevelName.Equals(TEXT("TutorialStage1_NM_Weapon"), ESearchCase::CaseSensitive);  //튜토리얼 맵용 획득저장 방지

        TArray<FName> RowNames = PlayerCharacter->WeaponDataTableRef->GetRowNames();
        for (FName RowName : RowNames)
        {
            // 무기 데이터 가져오기
            FWeaponDataTableBetaStruct* WeaponData = PlayerCharacter->WeaponDataTableRef->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);

            if (WeaponData && WeaponData->IID == OverlapID)
            {
                if (UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance()))
                {
                    // 튜토리얼 맵이 아닐 때만 true로 획득 기록
                    if (!bIsTutorialMap)
                    {
                        GI->Temp_Acquired.Add(FName(FString::FromInt(WeaponData->IID)), true);
                        UE_LOG(LogTemp, Warning, TEXT("WeaponGet (Temp_Acquired=true)"));
                    }
                    else
                    {
                        UE_LOG(LogTemp, Verbose, TEXT("Tutorial map detected (%s): skip Temp_Acquired Add(true)"),
                            *CurrentLevelName);
                    }
                }
                break;
            }
        }
        if (IsValid(OverlapWeapon)) {
            OverlapWeapon->DestroySelf();
        }
        PlayerCharacter->WeaponArray[HighlightedButtons[0]] = OverlapID;
        PlayerCharacter->WeaponSwap();
        PlayerCharacter->WeaponSwap();
        CloseWeaponExchange();
    }
    UE_LOG(LogTemp, Display, TEXT("AgreeButtonClicked"));
}

void UWeaponExchange_UI::OnDisagreeButtonClicked()
{
    CloseWeaponExchange();
    UE_LOG(LogTemp, Display, TEXT("DisAgreeButtonClicked"));
}
