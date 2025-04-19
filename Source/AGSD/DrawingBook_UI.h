// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "AccessoryData.h"
#include "DrawingBook_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UDrawingBook_UI : public UUserWidget
{
	GENERATED_BODY()

public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseDrawingBookButton;

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseDrawingBook();

    /** 악세서리 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UDataTable* AccessoryDataTable;
};
