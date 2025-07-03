// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponStruct
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString ID;

    // 획득 유무
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool IsAqcuired;

    // 강화 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsReinforce = false;

    //승천 횟수
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Ascension;

};