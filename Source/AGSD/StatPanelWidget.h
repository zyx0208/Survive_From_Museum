// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatPanelWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UImage;

/**
 * 
 */
UCLASS()
class AGSD_API UStatPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void SetupStatBars(class AAGSDCharacter* CharacterRef);

    UFUNCTION(BlueprintCallable)
    void ToggleStatBox();

protected:
    UPROPERTY(meta = (BindWidget))
    class UVerticalBox* StatListBox;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<class UStatBarWidget> StatBarClass;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_Attack;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_Defense;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_AS;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_AR;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_Speed;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_XPLevel;

    UPROPERTY(meta = (BindWidget))
    UStatBarWidget* StatBar_XPRange;

    // Stat TEXT
    UPROPERTY(meta = (BindWidget))
    UTextBlock* HPText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DefenseText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackSpeedText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* AttackRangeText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* SpeedText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DashText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* BounsXPText;
    UPROPERTY(meta = (BindWidget))
    UTextBlock* XPRangeText;

    // Stat Text Vertical Box
    UPROPERTY(meta = (BindWidget))
    UVerticalBox* StatListBox2;

    // Stat Image
    UPROPERTY(meta = (BindWidget))
    UImage* StatImage1;
    UPROPERTY(meta = (BindWidget))
    UImage* StatImage2;
    UPROPERTY(meta = (BindWidget))
    UImage* StatImage3;
};
