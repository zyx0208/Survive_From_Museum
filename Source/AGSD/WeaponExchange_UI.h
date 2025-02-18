// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "WeaponDataTableBeta.h"
#include "WeaponExchange_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UWeaponExchange_UI : public UUserWidget
{
	GENERATED_BODY()
	
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    /** 무기 아이콘을 UI에 표시 */
    void DisplayWeaponImage(int32 SlotIndex, UTexture2D* WeaponIcon);

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseWeaponExchange();

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponExchangeAgree;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponExchangedisagree;


    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot1;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot2;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlotNewWeapon;


private:
    /** 버튼 클릭 이벤트 핸들러 */
    UFUNCTION()
    void OnImageSlot1Clicked();

    UFUNCTION()
    void OnImageSlot2Clicked();

    /** 버튼 클릭 이벤트 */
    UFUNCTION()
    void OnImageSlotClicked(int32 ButtonIndex);

    UFUNCTION()
    void OnAgreeButtonClicked();

    UFUNCTION()
    void OnDisagreeButtonClicked();

    TArray<int32> HighlightedButtons;

};
