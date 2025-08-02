// Fill out your copyright notice in the Description page of Project Settings.


#include "AGSDCharacter_LevelUP.h"
#include "AGSDCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "AccessoryData.h" 
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
            FString ValueString = Effect.Mid(4).TrimStartAndEnd();
            float AttackIncrease = FCString::Atof(*ValueString);
            if (AttackIncrease != 0.0f)
            {
                ((Character->Attack + AttackIncrease) <= 0.0f) ? Character->Attack = 0.0f : Character->Attack += AttackIncrease;
                UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Current Attack: %.1f"), AttackIncrease, Character->Attack);
            }
        }
        else if (Effect.Contains(TEXT("공격속도")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float SpeedIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->AttackSpeedLevel += (SpeedIncrease / 100.0f);
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Speed %.1f"), SpeedIncrease, Character->AttackSpeedLevel);
        }
        else if (Effect.Contains(TEXT("공격범위")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float RangeIncrease = FCString::Atof(*ValueString.Replace(TEXT("+"), TEXT("")).TrimStartAndEnd());
            Character->AttackRangeLevel += RangeIncrease;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Range %.1f"), RangeIncrease, Character->AttackRangeLevel);
        }
        else if (Effect.Contains(TEXT("최대체력")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            int32 HealthIncrease = FCString::Atoi(*ValueString);
            
            ((Character->CurrentHealth + HealthIncrease) <= 1) ? Character->CurrentHealth = 1 : Character->CurrentHealth += HealthIncrease;
            ((Character->MaxHealth + HealthIncrease) <= 1) ? Character->MaxHealth = 1 : Character->MaxHealth += HealthIncrease;
            
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d Health %d / %d"), HealthIncrease, Character->CurrentHealth, Character->MaxHealth);
            Character->UpdateHealthBar();
        }
        else if (Effect.Contains(TEXT("체력회복")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float HealthIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            int32 tempHealth = Character->MaxHealth * (HealthIncrease / 100.0f);
            if (Character->CurrentHealth + tempHealth >= Character->MaxHealth)
            {
                Character->CurrentHealth = Character->MaxHealth;
            }
            else
            {
                Character->CurrentHealth += tempHealth;
            }
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d / %d"), Character->CurrentHealth, Character->MaxHealth);
            Character->UpdateHealthBar();
        }
        else if (Effect.Contains(TEXT("임시보호막")))
        {
            Character->Barrier += 10;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %d"), Character->Barrier);
            Character->UpdateHealthBar();
        }
        else if (Effect.Contains(TEXT("이동속도")))
        {
            FString ValueString = Effect.Mid(5).TrimStartAndEnd();
            float SpeedIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->SpeedLevel += 500.0f * (SpeedIncrease / 100.0f);
            Character->GetCharacterMovement()->MaxWalkSpeed = Character->SpeedLevel;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Speed %.1f"), SpeedIncrease, Character->SpeedLevel);
        }
        else if (Effect.Contains(TEXT("방어력")))
        {
            FString ValueString = Effect.Mid(4).TrimStartAndEnd();
            float DefenseIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            (Character->Defense + DefenseIncrease <= 0) ? Character->Defense = 0.0f : Character->Defense += DefenseIncrease;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Defense %.1f"), DefenseIncrease, Character->Defense);
        }
        else if (Effect.Contains(TEXT("대쉬 쿨타임")))
        {
            (Character->DashCooldown <= 2.0f) ? Character->DashCooldown = 2.0f : Character->DashCooldown -= 1.0f;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: 1sec DashCooldown %.1f"), Character->DashCooldown);
        }
        else if (Effect.Contains(TEXT("경험치 획득량")))
        {
            Character->BounsXPLevel += 0.20f;
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: +20 BounsXPLevel %.2f"), Character->BounsXPLevel);
        }
        else if (Effect.Contains(TEXT("경험치 획득반경")))
        {
            FString ValueString = Effect.Mid(9).TrimStartAndEnd();
            float RangeIncrease = FCString::Atof(*ValueString.Replace(TEXT("%"), TEXT("")));
            Character->XPRangeLevel += (RangeIncrease / 100.0f);
            Character->MagnetSphere->SetSphereRadius(XPRangeLevel);
            UE_LOG(LogTemp, Log, TEXT("Increase Effect: %.1f Range %.1f"), RangeIncrease, Character->XPRangeLevel);
        }
        else if (Effect.Contains(TEXT("보조무기")))
        {
            // 보조 무기 추가 처리
            Character->SpawnSubWeapon(Accessory.SubWeaponSelector);
        }
        else if (Effect.Contains(TEXT("부활")))
        {
            // 부활 및 잠시 무적
            UE_LOG(LogTemp, Log, TEXT("부활 및 1초 무적효과 적용"));
            Character->IsResurrection = true;
        }
        else if (Effect.Contains(TEXT("랜덤+")))
        {
            // 랜덤효과 적용
            UE_LOG(LogTemp, Log, TEXT("랜덤효과 적용+"));
            // 0~5 사이의 랜덤한 스탯 선택
            int32 RandomStat = FMath::RandRange(0, 6);
            RandmoStatApplyP(Character, RandomStat);
        }
        else if (Effect.Contains(TEXT("랜덤-")))
        {
            // 랜덤효과 적용
            UE_LOG(LogTemp, Log, TEXT("랜덤효과 적용-"));
            // 0~5 사이의 랜덤한 스탯 선택
            int32 RandomStat = FMath::RandRange(0, 6);
            RandmoStatApplyM(Character, RandomStat);
        }
        else if (Effect.Contains(TEXT("피격시 5초 무적")))
        {
            UE_LOG(LogTemp, Log, TEXT("피격시 5초 무적효과 적용"));
            Character->bIs_Attacked_Invincible = true;
        }
        else if (Effect.Contains(TEXT("가드")))
        {
            FString ValueString = Effect.Mid(3).TrimStartAndEnd(); // " 7회"
            ValueString.RemoveFromEnd(TEXT("회")); // "7"
            int32 GuardCount = FCString::Atoi(*ValueString);
            Character->Guard += GuardCount;
            UE_LOG(LogTemp, Log, TEXT("가드효과 적용: %d회"), GuardCount);
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("ERROR Effect"));
        }
    }
    Character->UpdateStat();
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

void AAGSDCharacter_LevelUP::RandmoStatApplyM(AAGSDCharacter* Character, int randomstat)
{
    switch (randomstat)
    {
    case 0: // MaxHealth 감소
        if (Character->MaxHealth > 5)  // 최소 체력 제한
        {
            Character->MaxHealth -= 5;
            Character->CurrentHealth -= 5;
            UE_LOG(LogTemp, Log, TEXT("MaxHealth 감소: %d"), Character->MaxHealth);
        }
        else
            Character->MaxHealth = 1;
        break;

    case 1: // Defense 감소
        if (Character->Defense >= 5.0f) // 최소 방어력 제한
        {
            Character->Defense -= 5.0f;
            UE_LOG(LogTemp, Log, TEXT("Defense 감소: %f"), Character->Defense);
        }
        else
            Character->Defense = 0.0f;
        break;

    case 2: // SpeedLevel 감소
        if (Character->SpeedLevel > 50.0f) // 최소 속도 제한
        {
            Character->SpeedLevel -= 50.0f; // 5% 감소(500.f 의 5%) 
            UE_LOG(LogTemp, Log, TEXT("SpeedLevel 감소: %.2f"), Character->SpeedLevel);
        }
        else
        {
            Character->SpeedLevel = 50.0f;
        }
        break;

    case 3: // Attack 감소
        if (Character->Attack >= 1.0f) // 최소 공격력 제한
        {
            Character->Attack -= 1.0f; // 10% 감소
            UE_LOG(LogTemp, Log, TEXT("Attack 감소: %.2f"), Character->Attack);
        }
        break;

    case 4: // 경험치획득량증가량 감소
        if (Character->BounsXPLevel > 0.2f) // 최소 제한
        {
            Character->BounsXPLevel -= 0.2f; // 20% 감소
            UE_LOG(LogTemp, Log, TEXT("BounsXPLevel 감소: %.2f"), Character->BounsXPLevel);
        }
        else
        {
            Character->BounsXPLevel = 0.2f;
        }
        break;

    case 5: // AttackSpeedLevel 감소
        if (Character->AttackSpeedLevel > 0.1f) // 최소 공격속도 제한
        {
            Character->AttackSpeedLevel -= 0.1f; // 10% 감소
            UE_LOG(LogTemp, Log, TEXT("AttackSpeedLevel 감소: %.2f"), Character->AttackSpeedLevel);
        }
        else
        {
            Character->AttackSpeedLevel = 0.1f;
        }
        break;

    case 6: // 경험치 획득 반경 감소
        if (Character->XPRangeLevel > 40.0f) //최소 제한
        {
            Character->XPRangeLevel -= 40.0f; //20% 감소
            UE_LOG(LogTemp, Log, TEXT("XPRangeLevel 감소: %.2f"), Character->XPRangeLevel);
        }
        else
        {
            Character->XPRangeLevel = 40.0f;
        }
        break;
    }
}

void AAGSDCharacter_LevelUP::RandmoStatApplyP(AAGSDCharacter* Character, int randomstat)
{
    switch (randomstat)
    {
    case 0: // MaxHealth 증가
        Character->MaxHealth += 5;
        Character->CurrentHealth += 5;
        UE_LOG(LogTemp, Log, TEXT("MaxHealth 증가: %d / %d"), Character->CurrentHealth, Character->MaxHealth);
        
        break;

    case 1: // Defense 증가
        
         Character->Defense += 5.0f;
        UE_LOG(LogTemp, Log, TEXT("Defense 증가: %f"), Character->Defense);
        
        break;

    case 2: // SpeedLevel 증가
        
        Character->SpeedLevel += 50.0f; // 10% 증가
        UE_LOG(LogTemp, Log, TEXT("SpeedLevel 증가: %.2f"), Character->SpeedLevel);
        
        break;

    case 3: // Attack 증가
        
        Character->Attack += 1.0f; // 10% 증가
        UE_LOG(LogTemp, Log, TEXT("Attack 증가: %.2f"), Character->Attack);
        
        break;

    case 4: // BounsXPLevel 증가
        
        Character->BounsXPLevel += 0.2f; // 20% 증가
        UE_LOG(LogTemp, Log, TEXT("BounsXPLevel 증가: %.2f"), Character->BounsXPLevel);
        
        break;

    case 5: // AttackSpeedLevel 증가
        
        Character->AttackSpeedLevel += 0.1f; // 10% 증가
        UE_LOG(LogTemp, Log, TEXT("AttackSpeedLevel 증가: %.2f"), Character->AttackSpeedLevel);

        break;

    case 6: //경험치 획득 반경 증가
        Character->XPRangeLevel += 40.0f;
        UE_LOG(LogTemp, Log, TEXT("XPRangeLevel 증가: %.2f"), Character->XPRangeLevel);
        break;
    }

    
}

