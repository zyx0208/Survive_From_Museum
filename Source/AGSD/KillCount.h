// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "KillCount.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AGSD_API UKillCount : public UObject
{
	GENERATED_BODY()
    UKillCount();
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int KillCount = 0;//적 처치 수

    UFUNCTION()
    void KillEnemy();//적 처치시 발동
};
