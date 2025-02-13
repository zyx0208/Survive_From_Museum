// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StorageBox_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UStorageBox_UI : public UUserWidget
{
	GENERATED_BODY()

protected:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

public:
    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseStorageBoxButton;

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseStorageBox();
};
