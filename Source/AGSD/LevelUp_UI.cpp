// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUp_UI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "AGSDCharacter.h"

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

	/*OptionTexts = {
		FText::FromString("모든 체력 회복"),
		FText::FromString("최대 체력 20 증가"),
		FText::FromString("공격력 1 증가"),
		FText::FromString("경험치 획득량 50% 증가"),
		FText::FromString("방어력 10% 증가")
	};*/

	TArray<int32> AllIndices = { 0,1,2,3,4 };
	OptionIndices.Empty();
	for (int i = 0; i < 3; i++)
	{
		int32 RandomIndex = FMath::RandRange(0, AllIndices.Num() - 1);
		OptionIndices.Add(AllIndices[RandomIndex]);
		AllIndices.RemoveAt(RandomIndex);
	}
	UpdateOptionTexts();
}

void ULevelUp_UI::UpdateOptionTexts()
{
	if (OptionIndices.Num() >= 3)
	{
		if (Option1_TextBlock)
		{
			Option1_TextBlock->SetText(OptionTexts[OptionIndices[0]]);
		}

		if (Option2_TextBlock)
		{
			Option2_TextBlock->SetText(OptionTexts[OptionIndices[1]]);
		}

		if (Option3_TextBlock)
		{
			Option3_TextBlock->SetText(OptionTexts[OptionIndices[2]]);
		}
	}
}

void ULevelUp_UI::OnOption1Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(OptionIndices[0]);
	}
}

void ULevelUp_UI::OnOption2Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(OptionIndices[1]);
	}
}

void ULevelUp_UI::OnOption3Clicked()
{
	if (AAGSDCharacter* PlayerCharacter = Cast<AAGSDCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter()))
	{
		PlayerCharacter->ApplyLevelUpOption(OptionIndices[2]);
	}
}