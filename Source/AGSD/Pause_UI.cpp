// Fill out your copyright notice in the Description page of Project Settings.

#include "Pause_UI.h"
#include "AGSDCharacter.h"
#include "Components/WrapBox.h"
#include "AccessoryData.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

void UPause_UI::NativeConstruct()
{
    Super::NativeConstruct();

    PlayerCharacter = Cast<AAGSDCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (AccessoryWrapBox && PlayerCharacter && AccessoryDataTable && AccessoryImageClass)
    {
        PopulateAccessoryIcons();
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


