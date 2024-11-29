// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameTimer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AGSD_API UGameTimer : public UObject
{
	GENERATED_BODY()
	
    UGameTimer();

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float InitTime;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FTimerHandle GameTimer;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int UITime;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString StringTime=" ";

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool TimeEnd = false;

    UFUNCTION(BlueprintCallable)
    void TimeSet();

    UFUNCTION(BlueprintCallable)
    void UpdateTime();

    UFUNCTION()
    void TimeOver();

    UFUNCTION()
    FString MinutesSeconds(int Time);
};
