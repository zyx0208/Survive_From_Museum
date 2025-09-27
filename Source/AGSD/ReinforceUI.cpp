// Fill out your copyright notice in the Description page of Project Settings.


#include "ReinforceUI.h"
#include "WeaponDataTableBeta.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "AGSDGameInstance.h"
#include "AGSDCharacter.h"

void UReinforceUI::NativeConstruct()
{
    if (AgreeButton) {
        AgreeButton->OnClicked.AddDynamic(this, &UReinforceUI::OnAgreeButtonClicked);
    }
    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
    if (WeaponDataTableBeta!=nullptr) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
        {
            
            FName RowName1 = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData1 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName1, ContextString, true);
            //UTexture2D* WeaponIcon1 = WeaponData1->WeaponIcon;

            FName RowName2 = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName2, ContextString, true);

            if (UpgradeCheck(*WeaponData1)) {
                ShowReinforce(*WeaponData1);
                Recheck = true;
            }
            else if (UpgradeCheck(*WeaponData2)) {
                WeaponArraySlot = true;
                ShowReinforce(*WeaponData2);
            }
            //DisplayWeaponImage(WeaponIcon1, WeaponIcon2);
        }
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("NO DataTable"));
        CloseUI();
    }
}

void UReinforceUI::DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2)
{
}

void UReinforceUI::CloseUI()
{
    /*
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }*/
    RemoveFromViewport();
}

void UReinforceUI::OnAgreeButtonClicked()
{
    // 튜토리얼 보스 맵에서는 강화 스킵 후 UI만 닫기
    const FString CurrentLevel = UGameplayStatics::GetCurrentLevelName(this, true);
    if (CurrentLevel == TEXT("TutorialStage4_BossMonster"))
    {
        UE_LOG(LogTemp, Log, TEXT("[ReinforceUI] Tutorial boss stage - skip reinforce and close UI."));
        TutorialTextWidget = CreateWidget<UUserWidget>(GetWorld(), TutorialTextWidgetClass);
        if (TutorialTextWidget) {
            TutorialTextWidget->AddToViewport();
        }
        CloseUI();
        return;
    }

    UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
    AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
    if (GI) {
        
        if (OriginalWeapon != nullptr && TargetWeapon != nullptr) {
            GI->Temp_Reinforced.Add(FName(FString::FromInt(OriginalWeapon->IID)), true);
            GI->Temp_Reinforced.Add(FName(FString::FromInt(TargetWeapon->IID)), true);
            GI->Temp_Acquired.Add(FName(FString::FromInt(OriginalWeapon->IID)), true);
            GI->Temp_Acquired.Add(FName(FString::FromInt(TargetWeapon->IID)), true);
            UE_LOG(LogTemp, Warning, TEXT("WeaponReinforce"));

        }
        
    }

    if (!WeaponArraySlot) {
        PlayerCharacter->WeaponArray[0] = TargetWeapon->IID;
        GI->WeaponArray[0] = TargetWeapon->IID;
        UE_LOG(LogTemp, Log, TEXT("%d 강화"),GI->WeaponArray[0]);
        WeaponArraySlot = true;
    }
    else {
        PlayerCharacter->WeaponArray[1] = TargetWeapon->IID;
        GI->WeaponArray[1] = TargetWeapon->IID;
        UE_LOG(LogTemp, Log, TEXT("%d 강화"), GI->WeaponArray[1]);
    }

    if (Recheck) {
        Recheck = false;
        
        FName RowName2 = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
        FWeaponDataTableBetaStruct* WeaponData2 = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName2, "Recheck", true);
        if (UpgradeCheck(*WeaponData2)) {
            ShowReinforce(*WeaponData2);
        }
        else {
            CloseUI();
        }
    }
    else
    {
        if (GI->NowStage == -1) {
            
        }
        CloseUI();
    }
}

bool UReinforceUI::UpgradeCheck(FWeaponDataTableBetaStruct WeaponData)
{
    FString ReinforceStageName = UGameplayStatics::GetCurrentLevelName(this, true);
    UE_LOG(LogTemp, Warning ,TEXT("%s // %s"), *WeaponData.ReinforceStage, *ReinforceStageName);
    if (WeaponData.ReinforceStage == ReinforceStageName) {
        return true;
    }
    else if ((WeaponData.IID == 4 || WeaponData.IID == 5) && ReinforceStageName == "TutorialStage4_BossMonster") {
        return true; //튜토용 찬란한 업그레이드
    }
    UE_LOG(LogTemp, Warning, TEXT("%d // %s"), WeaponData.IID, *ReinforceStageName);
    return false;
}

void UReinforceUI::Reinforce(FWeaponDataTableBetaStruct WeaponData)
{
}

void UReinforceUI::ShowReinforce(FWeaponDataTableBetaStruct WeaponData)
{
    

    static const FString ContextString(TEXT("Weapon Data Context"));
    TargetWeapon = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(FName(FString::FromInt(WeaponData.UpgradeID)), ContextString, true);
    OriginalWeapon = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(FName(FString::FromInt(WeaponData.IID)), ContextString, true);

    UTexture2D* WeaponIcon1 = OriginalWeapon->WeaponIcon;
    ImageSlot1->SetBrushFromTexture(WeaponIcon1);
    WeaponDescription1->SetText(FText::FromString(OriginalWeapon->WeaponDescription));
    WeaponNameTextBlock1->SetText(FText::FromString(OriginalWeapon->Sname));

    UTexture2D* WeaponIcon2 = TargetWeapon->WeaponIcon;
    ImageSlot2->SetBrushFromTexture(WeaponIcon2);
    WeaponDescription2->SetText(FText::FromString(TargetWeapon->WeaponDescription));
    WeaponNameTextBlock2->SetText(FText::FromString(TargetWeapon->Sname));
}
