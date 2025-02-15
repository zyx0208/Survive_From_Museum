// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "WeaponDataTableBeta.h"
#include "StorageBox_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UStorageBox_UI : public UUserWidget
{
	GENERATED_BODY()


public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* CloseStorageBoxButton;

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseStorageBox();


    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;

    /** 이미지 슬롯 */
    void DisplayWeaponImage(int32 SlotIndex, UTexture2D* WeaponIcon);

    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot1;

    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot2;

    /** 버튼 슬롯 */
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot1Button;
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnImageSlot1Clicked();

    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot2Button;
    UFUNCTION(BlueprintCallable, Category = "UI")
    void OnImageSlot2Clicked();



    
};
