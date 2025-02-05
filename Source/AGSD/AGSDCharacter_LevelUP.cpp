// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDCharacter_LevelUP.h"
#include "AGSDCharacter.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetStringLibrary.h"

void AAGSDCharacter_LevelUP::ApplyLevelUpOption(const FAccessoryData& SelectedAccessory)
{
    if (SelectedAccessory.AccessoryName.IsEmpty() || SelectedAccessory.AccessoryEffect.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Accessory Data."));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Selected Accessory: %s"), *SelectedAccessory.AccessoryName);
    ApplyAccessoryEffect(SelectedAccessory);
    ResumeGameAfterLevelUp();
}

void AAGSDCharacter_LevelUP::ApplyAccessoryEffect(const FAccessoryData& Accessory)
{
    if (Accessory.AccessoryEffect.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("AccessoryEffect is empty."));
        return;
    }

    TArray<FString> ParsedEffects;
    ParseAccessoryEffect(Accessory.AccessoryEffect, ParsedEffects);

    for (const FString& Effect : ParsedEffects)
    {
        if (Effect.Contains(TEXT("공격력")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float AttackIncrease = FCString::Atof(*ValueString);
            if (AttackIncrease != 0.0f)
            {
                Attack += AttackIncrease;
                UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Current Attack: %.1f"), AttackIncrease, Attack);
            }
        }
        else if (Effect.Contains(TEXT("최대체력")))
        {
            FString ValueString = Effect.RightChop(4);
            int32 HealthIncrease = FCString::Atoi(*ValueString);
            MaxHealth += HealthIncrease;
            CurrentHealth += HealthIncrease;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d Health %d / %d"), HealthIncrease, CurrentHealth, MaxHealth);
        }
        else if (Effect.Contains(TEXT("이동속도")))
        {
            FString ValueString = Effect.RightChop(6);
            float SpeedIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            SpeedLevel *= (1 + SpeedIncrease / 100.0f);
        }
        else if (Effect.Contains(TEXT("방어력")))
        {
            FString ValueString = Effect.RightChop(5);
            float DefenseIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Defense *= (1 + DefenseIncrease / 100.0f);
        }
        else if (Effect.Contains(TEXT("대쉬 쿨타임")))
        {
            DashCooldown -= 1.0f;
        }
        else if (Effect.Contains(TEXT("보조무기")))
        {
            // 보조 무기 추가 처리
        }
    }
}

void AAGSDCharacter_LevelUP::ParseAccessoryEffect(const FString& EffectString, TArray<FString>& OutEffects)
{
    TArray<FString> SplittedBySlash;
    EffectString.ParseIntoArray(SplittedBySlash, TEXT("/"), true);

    for (const FString& SubEffect : SplittedBySlash)
    {
        if (!SubEffect.IsEmpty())
        {
            OutEffects.Add(SubEffect);
        }
    }
}

