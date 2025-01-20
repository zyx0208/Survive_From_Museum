// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUp_UI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "AGSDCharacter.h"
#include "Kismet/KismetMathLibrary.h"

void ULevelUp_UI::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Option1)
	{
		Option1->OnClicked.AddDynamic(this, &ULevelUp_UI::OnOption1Clicked);
	}

	if (Option2)
	{
		Option2->OnClicked.AddDynamic(this, &ULevelUp_UI::OnOption2Clicked);
	}

	if (Option3)
	{
		Option3->OnClicked.AddDynamic(this, &ULevelUp_UI::OnOption3Clicked);
	}

    // 랜덤 선택지 생성
    SelectRandomAccessories();
	UpdateOptionTexts();
}

void ULevelUp_UI::SelectRandomAccessories()
{
    SelectedAccessories.Empty();

    if (!AccessoryDataTable) return;

    TArray<FName> RowNames = AccessoryDataTable->GetRowNames();
    TArray<FAccessoryData*> CommonItems, RareItems, LegendaryItems;

    // 데이터 테이블에서 항목 분류
    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT(""));
        if (Accessory)
        {
            switch (Accessory->Rarity)
            {
            case EAccessoryRarity::Common:
                CommonItems.Add(Accessory);
                break;
            case EAccessoryRarity::Rare:
                RareItems.Add(Accessory);
                break;
            case EAccessoryRarity::Legendary:
                LegendaryItems.Add(Accessory);
                break;
            }
        }
    }

    // 각 칸의 등급 확률에 따라 등급 선택
    auto GetRarityByProbability = []() -> EAccessoryRarity
        {
            float RandomValue = FMath::FRand() * 100.0f;
            if (RandomValue < 60.0f)
            {
                return EAccessoryRarity::Common; // 60%
            }
            else if (RandomValue < 90.0f)
            {
                return EAccessoryRarity::Rare; // 30%
            }
            else
            {
                return EAccessoryRarity::Legendary; // 10%
            }
        };

    // 각 칸의 등급을 결정하고 해당 등급에서 랜덤으로 장신구 선택
    for (int32 i = 0; i < 3; ++i)
    {
        EAccessoryRarity Rarity = GetRarityByProbability();

        TArray<FAccessoryData*>* ItemPool = nullptr;
        switch (Rarity)
        {
        case EAccessoryRarity::Common:
            ItemPool = &CommonItems;
            break;
        case EAccessoryRarity::Rare:
            ItemPool = &RareItems;
            break;
        case EAccessoryRarity::Legendary:
            ItemPool = &LegendaryItems;
            break;
        }

        if (ItemPool && ItemPool->Num() > 0)
        {
            int32 RandomIndex = FMath::RandRange(0, ItemPool->Num() - 1);
            SelectedAccessories.Add(*(*ItemPool)[RandomIndex]);
        }
    }
}

void ULevelUp_UI::UpdateOptionTexts()
{
    if (SelectedAccessories.Num() >= 3)
    {
        if (Option1_Name)
        {
            Option1_Name->SetText(FText::FromString(SelectedAccessories[0].AccessoryName));
            Option1_Effect->SetText(FText::FromString(SelectedAccessories[0].AccessoryEffect));
            if (SelectedAccessories[0].AccessoryIcon)
            {
                Option1_Img->SetBrushFromTexture(SelectedAccessories[0].AccessoryIcon);
            }
        }

        if (Option2_Name)
        {
            Option2_Name->SetText(FText::FromString(SelectedAccessories[1].AccessoryName));
            Option2_Effect->SetText(FText::FromString(SelectedAccessories[1].AccessoryEffect));
            if (SelectedAccessories[1].AccessoryIcon)
            {
                Option2_Img->SetBrushFromTexture(SelectedAccessories[1].AccessoryIcon);
            }
        }

        if (Option3_Name)
        {
            Option3_Name->SetText(FText::FromString(SelectedAccessories[2].AccessoryName));
            Option3_Effect->SetText(FText::FromString(SelectedAccessories[2].AccessoryEffect));
            if (SelectedAccessories[2].AccessoryIcon)
            {
                Option3_Img->SetBrushFromTexture(SelectedAccessories[2].AccessoryIcon);
            }
        }
    }
}

void ULevelUp_UI::OnOption1Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(SelectedAccessories[0]);
	}
}

void ULevelUp_UI::OnOption2Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(SelectedAccessories[1]);
	}
}

void ULevelUp_UI::OnOption3Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(SelectedAccessories[2]);
	}
}