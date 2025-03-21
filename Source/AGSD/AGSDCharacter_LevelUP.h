// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AGSDCharacter.h"
#include "AGSDCharacter_LevelUP.generated.h"
/**
 * 
 */
UCLASS()
class AGSD_API AAGSDCharacter_LevelUP : public AAGSDCharacter
{
	GENERATED_BODY()
public:
    /** 레벨업 시 선택한 옵션을 적용 */
    void ApplyLevelUpOption(AAGSDCharacter* Character, const FAccessoryData& SelectedAccessory);

protected:
    /** 선택된 장신구의 효과를 적용 */
    void ApplyAccessoryEffect(AAGSDCharacter* Character, const  FAccessoryData& Accessory);

    /** 장신구 효과 문자열을 파싱하여 분류 */
    void ParseAccessoryEffect(const FString& EffectString, TArray<FString>& OutEffects);

    void RandmoStatApply(AAGSDCharacter* Character, int randomstat);
};
