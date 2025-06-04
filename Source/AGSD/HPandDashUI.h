// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPandDashUI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UHPandDashUI : public UUserWidget
{
	GENERATED_BODY()

public:
    void UpdateSimpleHPBar(float HPper);
    void UpdateSimpleDashBar(float Dashper);

protected:
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* Simple_HPBar;
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* Simple_DashBar;
};
