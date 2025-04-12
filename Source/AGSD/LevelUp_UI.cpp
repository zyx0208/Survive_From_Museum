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
    //Effect Image 설정
    AccessoryRareEffect.Add(Option1RareEffect);
    AccessoryRareEffect.Add(Option2RareEffect);
    AccessoryRareEffect.Add(Option3RareEffect);
    AccessoryLegendaryEffect.Add(Option1LegendaryEffect);
    AccessoryLegendaryEffect.Add(Option2LegendaryEffect);
    AccessoryLegendaryEffect.Add(Option3LegendaryEffect);

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

    FString CurrentMapName = GetWorld()->GetMapName();
    CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    // 데이터 테이블에서 항목 분류
    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT(""));
       
        if (Accessory)
        {
            if ((Accessory->Theme == "Jelda" && CurrentMapName != "stage2") || 
                (Accessory->Theme == "Mario" && CurrentMapName != "stage3"))
            {
                continue; // 조건이 맞지 않으면 스킵
            }
            else if (Accessory->bIsRepetition || (!Accessory->bIsRepetition && !Accessory->bIsAcquired))
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
    }

    // 사용 가능한 아이템이 있는지 확인
    bool bHasCommon = CommonItems.Num() > 0;
    bool bHasRare = RareItems.Num() > 0;
    bool bHasLegendary = LegendaryItems.Num() > 0;
    if (!bHasCommon && !bHasRare && !bHasLegendary)
    {
        UE_LOG(LogTemp, Warning, TEXT("No accessories found in the data table!"));
        return;
    }

    // 각 칸의 등급 확률에 따라 등급 선택 (해당 등급에 아이템이 없으면 다른 등급 선택)
    auto GetValidRarity = [&](EAccessoryRarity PreferredRarity) -> EAccessoryRarity
        {
            if (PreferredRarity == EAccessoryRarity::Legendary && !bHasLegendary)
            {
                PreferredRarity = bHasRare ? EAccessoryRarity::Rare : EAccessoryRarity::Common;
            }
            if (PreferredRarity == EAccessoryRarity::Rare && !bHasRare)
            {
                PreferredRarity = bHasCommon ? EAccessoryRarity::Common : EAccessoryRarity::Legendary;
            }
            if (PreferredRarity == EAccessoryRarity::Common && !bHasCommon)
            {
                PreferredRarity = bHasRare ? EAccessoryRarity::Rare : EAccessoryRarity::Legendary;
            }
            return PreferredRarity;
        };

    // 등급을 결정하는 함수
    auto GetRarityByProbability = [&]() -> EAccessoryRarity
        {
            float RandomValue = FMath::FRand() * 100.0f;
            if (RandomValue < 60.0f) return GetValidRarity(EAccessoryRarity::Common);
            else if (RandomValue < 90.0f) return GetValidRarity(EAccessoryRarity::Rare);
            else return GetValidRarity(EAccessoryRarity::Legendary);
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
            UpdateAccessoryVFX(i, Rarity);
            break;
        case EAccessoryRarity::Rare:
            ItemPool = &RareItems;
            UpdateAccessoryVFX(i, Rarity);
            break;
        case EAccessoryRarity::Legendary:
            ItemPool = &LegendaryItems;
            UpdateAccessoryVFX(i, Rarity);
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
            Option1_Effect->SetText(FText::FromString(SelectedAccessories[0].AccessoryDescription));
            if (SelectedAccessories[0].AccessoryIcon)
            {
                Option1_Img->SetBrushFromTexture(SelectedAccessories[0].AccessoryIcon);
            }
        }

        if (Option2_Name)
        {
            Option2_Name->SetText(FText::FromString(SelectedAccessories[1].AccessoryName));
            Option2_Effect->SetText(FText::FromString(SelectedAccessories[1].AccessoryDescription));
            if (SelectedAccessories[1].AccessoryIcon)
            {
                Option2_Img->SetBrushFromTexture(SelectedAccessories[1].AccessoryIcon);
            }
        }

        if (Option3_Name)
        {
            Option3_Name->SetText(FText::FromString(SelectedAccessories[2].AccessoryName));
            Option3_Effect->SetText(FText::FromString(SelectedAccessories[2].AccessoryDescription));
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
		PlayerCharacter->ApplyLevelUpOption(PlayerCharacter, SelectedAccessories[0]);     
        FName RowName = SelectedAccessories[0].RowName;
        FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption1Clicked"));
        AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::OnOption2Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(PlayerCharacter, SelectedAccessories[1]);    
        FName RowName = SelectedAccessories[1].RowName;
        FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption2Clicked"));
        AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::OnOption3Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(PlayerCharacter, SelectedAccessories[2]);
        FName RowName = SelectedAccessories[2].RowName;
        FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption3Clicked"));
        AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::UpdateAccessoryVFX(int index, EAccessoryRarity rarity)
{
    switch (rarity)
    {
    case EAccessoryRarity::Common:
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    case EAccessoryRarity::Rare:
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Visible);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    case EAccessoryRarity::Legendary:
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Visible);
        break;
    default:
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    }
}

