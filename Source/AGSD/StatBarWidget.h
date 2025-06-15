// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UStatBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void InitializeStat(FText InStatName, float InValue, float InStartOffset, float InUnitValue);

    UFUNCTION(BlueprintCallable)
    void UpdateBarDisplay();


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ExposeOnSpawn))
    UTexture2D* AttackStatIconTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ExposeOnSpawn))
    UTexture2D* DefenseStatIconTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ExposeOnSpawn))
    UTexture2D* ASStatIconTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ExposeOnSpawn))
    UTexture2D* ARStatIconTexture;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat", meta = (ExposeOnSpawn))
    UTexture2D* SpeedStatIconTexture;


protected:
    virtual void NativeConstruct() override;

    // UMG 바인딩
    UPROPERTY(meta = (BindWidget))
    class UImage* StatIconImage;

    UPROPERTY(meta = (BindWidget))
    class UHorizontalBox* BarContainer;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StatBar")
    TSubclassOf<UUserWidget> BarClass;

    // 내부 데이터
    FText StatName;
    float CurrentValue;
    float StartOffset;
    float UnitValue;
    float LastValue = 0.f;

    //int32 MaxBars = 20;
};
