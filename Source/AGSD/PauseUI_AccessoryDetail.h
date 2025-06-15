// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AccessoryData.h"
#include "PauseUI_AccessoryDetail.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class AGSD_API UPauseUI_AccessoryDetail : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /** 텍스트 초기화 함수 */
    UFUNCTION(BlueprintCallable, Category = "Accessory")
    void SetAccessoryDetail(const FAccessoryData& InData);

protected:
    /** 바인딩된 텍스트 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* NameText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* EffectText;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* DescriptionText;
};
