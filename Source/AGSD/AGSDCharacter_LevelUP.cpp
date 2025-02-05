// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDCharacter_LevelUP.h"
#include "AGSDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetStringLibrary.h"

void AAGSDCharacter_LevelUP::ApplyLevelUpOption(AAGSDCharacter* Character, const FAccessoryData& SelectedAccessory)
{
    if (!Character)
    {
        UE_LOG(LogTemp, Error, TEXT("ParentCharacter is NULL"));
        return;
    }
    if (SelectedAccessory.AccessoryName.IsEmpty() || SelectedAccessory.AccessoryEffect.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid Accessory Data."));
        return;
    }
    UE_LOG(LogTemp, Log, TEXT("Selected Accessory: %s"), *SelectedAccessory.AccessoryName);
    ApplyAccessoryEffect(Character, SelectedAccessory);

    Character->ResumeGameAfterLevelUp();
}

void AAGSDCharacter_LevelUP::ApplyAccessoryEffect(AAGSDCharacter* Character, const FAccessoryData& Accessory)
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
                Character->Attack += AttackIncrease;
                UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Current Attack: %.1f"), AttackIncrease, Character->Attack);
            }
        }
        else if (Effect.Contains(TEXT("공격속도")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float SpeedIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->AttackSpeedLevel *= (1 + SpeedIncrease / 100.0f);
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Speed %.1f"), SpeedIncrease, Character->AttackSpeedLevel);
        }
        else if (Effect.Contains(TEXT("공격범위")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float RangeIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->AttackRangeLevel *= (1 + RangeIncrease / 100.0f);
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Range %.1f"), RangeIncrease, Character->AttackRangeLevel);
        }
        else if (Effect.Contains(TEXT("최대체력")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            int32 HealthIncrease = FCString::Atoi(*ValueString);
            Character->MaxHealth += HealthIncrease;
            Character->CurrentHealth += HealthIncrease;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d Health %d / %d"), HealthIncrease, Character->CurrentHealth, Character->MaxHealth);
        }
        else if (Effect.Contains(TEXT("임시보호막")))
        {
            Character->CurrentHealth += 10;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: 10 Health %d / %d"), Character->CurrentHealth, Character->MaxHealth);
        }
        else if (Effect.Contains(TEXT("이동속도")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float SpeedIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->SpeedLevel *= (1 + SpeedIncrease / 100.0f);
            Character->GetCharacterMovement()->MaxWalkSpeed = Character->SpeedLevel;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Speed %.1f"), SpeedIncrease, Character->SpeedLevel);
        }
        else if (Effect.Contains(TEXT("방어력")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            int32 DefenseIncrease = FCString::Atoi(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->Defense += DefenseIncrease;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d Defense %d"), DefenseIncrease, Character->Defense);
        }
        else if (Effect.Contains(TEXT("대쉬 쿨타임")))
        {
            Character->DashCooldown -= 1.0f;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: 1sec DashCooldown %.1f"), Character->DashCooldown);
        }
        else if (Effect.Contains(TEXT("경험치 획득량")))
        {
            Character->BounsXPLevel += 0.25f;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: +25 BounsXPLevel %.2f"), Character->BounsXPLevel);
        }
        else if (Effect.Contains(TEXT("경험치 획득반경")))
        {
            FString ValueString = Effect.Mid(9).TrimStartAndEnd();
            float RangeIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->XPRangeLevel *= (1 + RangeIncrease / 100.0f);
            Character->MagnetSphere->SetSphereRadius(XPRangeLevel);
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Range %.1f"), RangeIncrease, Character->XPRangeLevel);
        }
        else if (Effect.Contains(TEXT("보조무기")))
        {
            // 보조 무기 추가 처리
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("ERROR Effect"));
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

