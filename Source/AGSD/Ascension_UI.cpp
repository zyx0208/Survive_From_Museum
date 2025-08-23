// Fill out your copyright notice in the Description page of Project Settings.


#include "Ascension_UI.h"
#include "WeaponDataTableBeta.h"
#include "WeaponDrop.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "AGSDGameInstance.h"
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
        DisplayTextBlock(TextBlock1);
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
        if (!WeaponRow) continue;
        if (WeaponRow->IID-1 == FCString::Atoi(*CurrentCharacter->OverlapID))
        {
            UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
            if (GI) {
                if (GI->Temp_Ascension.Contains(FName(FString::FromInt(WeaponRow->IID-1)))) {
                    int& WeaponAscension = GI->Temp_Ascension[FName(FString::FromInt(WeaponRow->IID-1))];
                    WeaponAscension += 1;
                }
                
            }
        }
    }
    if (CurrentCharacter->OverlapWeaponDrop) {
        CurrentCharacter->OverlapWeaponDrop->DestroySelf();
    }
    CloseUI();
}

void UAscension_UI::DisplayTextBlock(UTextBlock* textBlock)
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
        if (!WeaponRow) continue;
        if (WeaponRow->IID - 1 == FCString::Atoi(*CurrentCharacter->OverlapID))
        {
            UAGSDGameInstance* GI = Cast<UAGSDGameInstance>(GetGameInstance());
            if (GI) {
                if (GI->Temp_Ascension.Contains(FName(FString::FromInt(WeaponRow->IID - 1)))) {
                    int& WeaponAscension = GI->Temp_Ascension[FName(FString::FromInt(WeaponRow->IID - 1))];
                    WeaponAscension += 1;
                }

                FFormatNamedArguments Args;
                Args.Add(TEXT("Name"), FText::FromString(WeaponRow->Sname));

                Args.Add(TEXT("Ascension1"), GI->Temp_Ascension[FName(FString::FromInt(WeaponRow->IID - 1))]);
                Args.Add(TEXT("Ascension2"), GI->Temp_Ascension[FName(FString::FromInt(WeaponRow->IID - 1))]+1);

                if (textBlock) {
                    textBlock->SetText(FText::Format(NSLOCTEXT("MynameSpace","AscensionFormat","{Name}+{Ascension1}이(가) {Name}+{Ascension2}로 강화됩니다."),Args));
                }
            }
        }
    }
    
}
