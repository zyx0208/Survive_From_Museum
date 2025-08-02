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
    AccessoryCommonEffect.Add(Option1CommonEffect);
    AccessoryCommonEffect.Add(Option2CommonEffect);
    AccessoryCommonEffect.Add(Option3CommonEffect);
    AccessoryRareEffect.Add(Option1RareEffect);
    AccessoryRareEffect.Add(Option2RareEffect);
    AccessoryRareEffect.Add(Option3RareEffect);
    AccessoryLegendaryEffect.Add(Option1LegendaryEffect);
    AccessoryLegendaryEffect.Add(Option2LegendaryEffect);
    AccessoryLegendaryEffect.Add(Option3LegendaryEffect);

    // 비활성화
    if (Option1) Option1->SetIsEnabled(false);
    if (Option2) Option2->SetIsEnabled(false);
    if (Option3) Option3->SetIsEnabled(false);
    // 1초 후 버튼 활성화
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([this](float DeltaTime)
        {
            EnableButtons(); // 1초 뒤에 호출될 함수
            return false;    // 한 번만 실행
        }), 1.0f); // 실시간 기준 1초

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
    AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
    bool tmpAcquiredAccessory = false;

    FString CurrentMapName = GetWorld()->GetMapName();
    CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    // 데이터 테이블에서 항목 분류
    for (FName RowName : RowNames)
    {
        FAccessoryData* Accessory = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT(""));
        
        if (PlayerCharacter && PlayerCharacter->AcquiredAccessories.Contains(RowName))
            tmpAcquiredAccessory = true;
        else
            tmpAcquiredAccessory = false;

        if (Accessory)
        {
            //Jaru, Elyon, JK, 
            if ((Accessory->Theme == "Elyon" && CurrentMapName != "Level2Stage") || 
                (Accessory->Theme == "JK" && CurrentMapName != "Level3Stage") ||
                (Accessory->Theme == "Togetherium" && CurrentMapName != "Level4Stage") ||
                (Accessory->Theme == "SteelRoid" && CurrentMapName != "Level5Stage"))
            {
                continue; // 조건이 맞지 않으면 스킵
            }
            else if (Accessory->bIsRepetition || (!Accessory->bIsRepetition && !tmpAcquiredAccessory))
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
            FAccessoryData* SelectedItem = nullptr;
            int32 Attempts = 0;
            const int32 MaxAttempts = 10;
            do
            {
                int32 RandomIndex = FMath::RandRange(0, ItemPool->Num() - 1);
                SelectedItem = (*ItemPool)[RandomIndex];
                ++Attempts;
            } while (SelectedItem && SelectedAccessories.Contains(*SelectedItem) && Attempts < MaxAttempts);

            if (SelectedItem && !SelectedAccessories.Contains(*SelectedItem))
            {
                SelectedAccessories.Add(*SelectedItem);
            }
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
            Option1_Effect->SetText(FText::FromString(SelectedAccessories[0].AccessoryEffect.Replace(TEXT("/"),TEXT("\n"))));
            Option1_Description->SetText(FText::FromString(SelectedAccessories[0].AccessoryDescription));
            if (SelectedAccessories[0].AccessoryIcon)
            {
                Option1_Img->SetBrushFromTexture(SelectedAccessories[0].AccessoryIcon);
            }
        }

        if (Option2_Name)
        {
            Option2_Name->SetText(FText::FromString(SelectedAccessories[1].AccessoryName));
            Option2_Effect->SetText(FText::FromString(SelectedAccessories[1].AccessoryEffect.Replace(TEXT("/"), TEXT("\n"))));
            Option2_Description->SetText(FText::FromString(SelectedAccessories[1].AccessoryDescription));
            if (SelectedAccessories[1].AccessoryIcon)
            {
                Option2_Img->SetBrushFromTexture(SelectedAccessories[1].AccessoryIcon);
            }
        }

        if (Option3_Name)
        {
            Option3_Name->SetText(FText::FromString(SelectedAccessories[2].AccessoryName));
            Option3_Effect->SetText(FText::FromString(SelectedAccessories[2].AccessoryEffect.Replace(TEXT("/"), TEXT("\n"))));
            Option3_Description->SetText(FText::FromString(SelectedAccessories[2].AccessoryDescription));
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

        PlayerCharacter->PlayingGetAccessoryRowName(RowName);
        PlayerCharacter->AcquiredAccessories.Add(RowName);
        PlayerCharacter->AccessoryKnowRowName(RowName);

        //FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption1Clicked"));
        UnableButtons();
        //AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::OnOption2Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(PlayerCharacter, SelectedAccessories[1]);    
        FName RowName = SelectedAccessories[1].RowName;
        PlayerCharacter->PlayingGetAccessoryRowName(RowName);
        PlayerCharacter->AcquiredAccessories.Add(RowName);
        PlayerCharacter->AccessoryKnowRowName(RowName);

        //FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption2Clicked"));
        UnableButtons();
        //AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::OnOption3Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(PlayerCharacter, SelectedAccessories[2]);
        FName RowName = SelectedAccessories[2].RowName;
        PlayerCharacter->PlayingGetAccessoryRowName(RowName);
        PlayerCharacter->AcquiredAccessories.Add(RowName);
        PlayerCharacter->AccessoryKnowRowName(RowName);

        //FAccessoryData* AccessoryPtr = AccessoryDataTable->FindRow<FAccessoryData>(RowName, TEXT("OnOption3Clicked"));
        UnableButtons();
        //AccessoryPtr->bIsAcquired = true;
	}
}

void ULevelUp_UI::UpdateAccessoryVFX(int index, EAccessoryRarity rarity)
{
    switch (rarity)
    {
    case EAccessoryRarity::Common:
        AccessoryCommonEffect[index]->SetVisibility(ESlateVisibility::Visible);
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    case EAccessoryRarity::Rare:
        AccessoryCommonEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Visible);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    case EAccessoryRarity::Legendary:
        AccessoryCommonEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Visible);
        break;
    default:
        AccessoryCommonEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryRareEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        AccessoryLegendaryEffect[index]->SetVisibility(ESlateVisibility::Hidden);
        break;
    }
}

void ULevelUp_UI::EnableButtons()
{
    if (Option1) Option1->SetIsEnabled(true);
    if (Option2) Option2->SetIsEnabled(true);
    if (Option3) Option3->SetIsEnabled(true);
}

void ULevelUp_UI::UnableButtons()
{
    if (Option1) Option1->SetIsEnabled(false);
    if (Option2) Option2->SetIsEnabled(false);
    if (Option3) Option3->SetIsEnabled(false);
}

