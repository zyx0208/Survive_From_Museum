// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AccessoryData.h"
#include "AccessoryDetail_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UAccessoryDetail_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void SetAccessory(const FAccessoryData& InData);
    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseAccessoryDetailButton;
    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseAccessoryDetail();

    UPROPERTY()
    class UDrawingBook_UI* DrawingBookRef;  //UI 상위 참조

    void SetDrawingBookRef(UDrawingBook_UI* InRef) { DrawingBookRef = InRef; }

protected:
    UPROPERTY(meta = (BindWidget))
    class UImage* AccessoryIconImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AccessoryNameText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AccessoryEffectText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AccessoryDescriptionText;
    
private:
    FAccessoryData AccessoryData;
};
