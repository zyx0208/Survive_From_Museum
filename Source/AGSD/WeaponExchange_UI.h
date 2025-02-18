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
	

public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    /** 무기 아이콘을 UI에 표시 */
    void DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2, UTexture2D* WeaponIconNewWeapon);

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseWeaponExchange();

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponExchangeAgreeButton;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* WeaponExchangedisagreeButton;


    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot1;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot2;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlotNewWeapon;

    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot1Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot2Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlotNewWeaponButton;

    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;

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
