// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Damage_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UDamage_UI : public UUserWidget
{
	GENERATED_BODY()
public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    //피해량 표기할 텍스트
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DamageText;
    UPROPERTY(meta = (BindWidget))
    TArray<UTextBlock*> DamageTextArray;
    UPROPERTY(meta = (BindWidget))
    UCanvasPanel* RootCanvas;
    //피해량 표기
    UFUNCTION()
    void DamageTextCreate(float damage, FVector2D screenPosition);
};
