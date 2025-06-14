// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatPanelWidget.generated.h"

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
};
