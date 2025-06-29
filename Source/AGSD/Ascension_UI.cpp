// Fill out your copyright notice in the Description page of Project Settings.


#include "Ascension_UI.h"
#include "WeaponDataTableBeta.h"
#include "WeaponDrop.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "AGSDCharacter.h"
void UAscension_UI::NativeConstruct()
{
    Super::NativeConstruct();

    if (AgreeButton) {
        AgreeButton->OnClicked.AddDynamic(this, &UAscension_UI::OnAgreeButtonClicked);
    }
    AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
    if (PlayerCharacter) {
        WeaponDataTableBeta = PlayerCharacter->WeaponDataTableRef;
    }
    

    if (WeaponDataTableBeta) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        FName RowName = FName(PlayerCharacter->OverlapID);
        FWeaponDataTableBetaStruct* WeaponData = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
        UTexture2D* WeaponIcon1 = WeaponData->WeaponIcon;

        DisplayWeaponImage(WeaponIcon1);
    }
}

void UAscension_UI::DisplayWeaponImage(UTexture2D* WeaponIcon1)
{
    if (ImageSlot1) {
        ImageSlot1->SetBrushFromTexture(WeaponIcon1);
    }
    
}

void UAscension_UI::CloseUI()
{
    if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
    {
        PlayerCharacter->ResumeGameAfterLevelUp();
    }
}


void UAscension_UI::OnAgreeButtonClicked()
{
    if (!WeaponDataTableBeta) return;

    //캐릭터 무기 교체(무기 강화)
    AAGSDCharacter* CurrentCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!CurrentCharacter) return;

    // 무기 강화(데이터 테이블 변화) 
    static const FString ContextString(TEXT("ReinforceWeapon"));
    TArray<FName> RowNames = WeaponDataTableBeta->GetRowNames();

    for (FName RowName : RowNames)
    {
        FWeaponDataTableBetaStruct* WeaponRow = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
        if (!WeaponRow) return;
        if (WeaponRow->IID == FCString::Atoi(*CurrentCharacter->OverlapID))
        {
            WeaponRow->Ascension += 1;
            UE_LOG(LogTemp, Warning, TEXT("Weapon %s is now Ascension Ascension: %d"), *RowName.ToString(), WeaponRow->Ascension);
        }
    }

    CloseUI();
}
