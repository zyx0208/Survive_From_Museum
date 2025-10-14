// Fill out your copyright notice in the Description page of Project Settings.


#include "ResultUI.h"
#include "AGSDGameMode.h"
#include "AGSDCharacter.h"
#include "Kismet/GameplayStatics.h"

void UResultUI::NativeConstruct()
{
    Super::NativeConstruct();
    PlayerCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (CloseResultButton)
    {
        CloseResultButton->OnClicked.AddDynamic(this, &UResultUI::CloseResult);
        CloseResultButton->SetIsEnabled(false);
    }
    if (WeaponDataTable) {
        static const FString ContextString(TEXT("Weapon Data Context"));
        if (PlayerCharacter)
        {
            FName RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[0]));
            FWeaponDataTableBetaStruct* WeaponData = WeaponDataTable->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* Icon1 = WeaponData->WeaponIcon;

            RowName = FName(FString::FromInt(PlayerCharacter->WeaponArray[1]));
            WeaponData = WeaponDataTable->FindRow<FWeaponDataTableBetaStruct>(RowName, ContextString, true);
            UTexture2D* Icon2 = WeaponData->WeaponIcon;

            DisplayWeaponImage(Icon1, Icon2);
        }
    }
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
        {
            CloseResultButton->SetIsEnabled(true); // 1초 뒤에 호출될 함수
            return false;    // 한 번만 실행
        }), 1.0f); // 실시간 기준 1초
    
    PopulateAccessoryIcons();
}

void UResultUI::CloseResult()
{
    AAGSDGameMode* Gamemode = GetWorld() ? Cast<AAGSDGameMode>(GetWorld()->GetAuthGameMode<AAGSDGameMode>()) : nullptr;
    if (Gamemode)
    {
        //Gamemode->ResetAccessoryData();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameMode 찾을 수 없음"));
    }
}

void UResultUI::DisplayWeaponImage(UTexture2D* Icon1, UTexture2D* Icon2)
{
    if (WeaponIcon1) {
        WeaponIcon1->SetBrushFromTexture(Icon1);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot1"));
    }

    if (WeaponIcon2) {
        WeaponIcon2->SetBrushFromTexture(Icon2);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("not fountslot2"));
    }
}

void UResultUI::PopulateAccessoryIcons()
{
    AccessoryWrapBox->ClearChildren();
    

    for (const FName& RowName : PlayerCharacter->GetAccessory)
    {
        const FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("ResultUI"));

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