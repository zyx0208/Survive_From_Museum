// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "WeaponDataTableBeta.h"
#include "Ascension_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UAscension_UI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    /** 무기 아이콘을 UI에 표시 */
    void DisplayWeaponImage(UTexture2D* WeaponIcon1);

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseUI();

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* AgreeButton;

    UPROPERTY(meta = (BindWidget))
    class UImage* ImageSlot1;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TextBlock1;

    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;

    UFUNCTION()
    void OnAgreeButtonClicked();

    UFUNCTION()
    void DisplayTextBlock(UTextBlock* textBlock);
};
