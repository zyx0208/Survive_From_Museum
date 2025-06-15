// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatUnitBar.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UStatUnitBar : public UUserWidget
{
	GENERATED_BODY()

protected:
    UPROPERTY(meta = (BindWidgetAnim), Transient)
    UWidgetAnimation* Blink;

public:
    UFUNCTION(BlueprintCallable, Category = "Blink")
    void PlayBlink();
};
