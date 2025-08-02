// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "AccessoryData.h"
#include "AccessoryIcon_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UAccessoryIcon_UI : public UUserWidget
{
	GENERATED_BODY()
public:
    // AccessoryIcon_UI.h
    UPROPERTY(meta = (BindWidget))
    UButton* IconButton;
    UPROPERTY(meta = (BindWidget))
    UImage* IconImage;
    
    UPROPERTY(BlueprintReadOnly)
    FAccessoryData AccessoryData;

    UFUNCTION()
    void OnIconClicked();

    void Init(FAccessoryData* InData);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> AccessoryDetailClass;

    UPROPERTY()
    class UDrawingBook_UI* DrawingBookRef;

    void SetDrawingBookRef(UDrawingBook_UI* InRef) { DrawingBookRef = InRef; }

    //각 등급별 버튼 이미지
    UPROPERTY(meta = (BindWidget))
    UImage* CommonEffect;

    UPROPERTY(meta = (BindWidget))
    UImage* RareEffect;

    UPROPERTY(meta = (BindWidget))
    UImage* LegendaryEffect;
};
