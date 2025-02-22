// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h" // UTexture2D를 위해 필요
#include "SubWeapon.h"
#include "AccessoryData.generated.h"

// 등급 Enum 정의
UENUM(BlueprintType)
enum class EAccessoryRarity : uint8
{
    Common UMETA(DisplayName = "Common"),
    Rare UMETA(DisplayName = "Rare"),
    Legendary UMETA(DisplayName = "Legendary")
};

// 장신구 데이터 구조체
USTRUCT(BlueprintType)
struct FAccessoryData : public FTableRowBase
{
    GENERATED_BODY()

    // 장신구 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    FName RowName;

    // 장신구 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    FString AccessoryName;

    // 효과
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    FString AccessoryEffect;

    // 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    FString AccessoryDescription;

    // 등장 확률
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    float AccessoryProbability;

    // 등급
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    EAccessoryRarity Rarity;

    // 테마
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    FString Theme;

    // 획득 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    bool bIsAcquired = false;

    // 중복 가능 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    bool bIsRepetition = false;

    // 아이콘 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    UTexture2D* AccessoryIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Accessory")
    TSubclassOf<ASubWeapon> SubWeaponSelector;
};
