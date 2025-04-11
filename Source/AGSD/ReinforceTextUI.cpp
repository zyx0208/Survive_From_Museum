// Fill out your copyright notice in the Description page of Project Settings.


#include "ReinforceTextUI.h"
#include "AGSDGameInstance.h"        // AGSDGameInstance를 포함하는 헤더
#include "Kismet/GameplayStatics.h"   // GetPlayerCharacter를 사용하기 위함
#include "AGSDCharacter.h"            // AAGSDCharacter 관련
#include "Engine/DataTable.h"         // UDataTable 관련


void UReinforceTextUI::ReinforceWeapon()
{
    if (!WeaponDataTableBeta) return;
    UAGSDGameInstance* GameInstance = Cast<UAGSDGameInstance>(GetGameInstance());
    if (!GameInstance) {
        UE_LOG(LogTemp, Warning, TEXT("GameInstance is invalid"));
        return;
    }
    int32 ReinforceWeaponIndex = GameInstance->ReinforceWeaponIndex;
    int32 setIndex = 0;
    //무기 강화를 위한 인덱스 매칭(IID값) #무기업그레이드
    //WeaponIndexSetArray = { 4, 5, 9, 7, 10, 6, 8, 1 }; //무기 기초 배열(IID값)
    //UpgradeWeaponIndexSetArray = { 0, 0, 11, 0, 0, 0, 0, 0 };   //무기 업그레이드시 IID값 변경 설정    
    switch (ReinforceWeaponIndex)
    {
    case 9:
        setIndex = 11;
        break;
    default:
        break;
    }
    // 무기 강화(데이터 테이블 변화) 
    static const FString ContextString(TEXT("ReinforceWeapon"));
    TArray<FName> RowNames = WeaponDataTableBeta->GetRowNames();

    for (FName RowName : RowNames)
    {
        FWeaponDataTableBetaStruct* WeaponRow = WeaponDataTableBeta->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
        if (WeaponRow && WeaponRow->IID == ReinforceWeaponIndex)
        {
            WeaponRow->bIsAcquired = true;
            WeaponRow->bIsReinforce = true;
            UE_LOG(LogTemp, Warning, TEXT("Weapon %s is now reinforced"), *RowName.ToString());
            break;
        }
    }

    //캐릭터 무기 교체(무기 강화)
    AAGSDCharacter* CurrentCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (!CurrentCharacter) return;
    if (CurrentCharacter->WeaponArray[0] == ReinforceWeaponIndex)
    {
        CurrentCharacter->WeaponArray[0] = setIndex;
    }
    else
    {
        CurrentCharacter->WeaponArray[1] = setIndex;
    }
    CurrentCharacter->WeaponSwap();
    CurrentCharacter->WeaponSwap();
}