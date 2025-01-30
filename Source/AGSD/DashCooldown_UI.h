// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashCooldown_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UDashCooldown_UI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /** UI 업데이트 함수 */
    UFUNCTION(BlueprintCallable, Category = "Cooldown")
    void UpdateDashCooldown(float CooldownPercentage);

protected:
    /** 위젯 바인딩 */
    UPROPERTY(meta = (BindWidget))
    class UImage* DashIcon;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* DashCooldownBar;
};
