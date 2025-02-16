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

    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot1;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot2;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot3;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot4;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot5;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot6;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot7;
    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot8;

    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot1Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot2Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot3Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot4Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot5Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot6Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot7Button;
    UPROPERTY(meta = (BindWidget))
    UButton* ImageSlot8Button;

    /** 무기 아이콘을 UI에 표시 */
    void DisplayWeaponImage(int32 SlotIndex, UTexture2D* WeaponIcon);

private:
    /** 버튼 클릭 이벤트 핸들러 */
    UFUNCTION()
    void OnImageSlot1Clicked();

    UFUNCTION()
    void OnImageSlot2Clicked();

    UFUNCTION()
    void OnImageSlot3Clicked();

    UFUNCTION()
    void OnImageSlot4Clicked();

    UFUNCTION()
    void OnImageSlot5Clicked();

    UFUNCTION()
    void OnImageSlot6Clicked();

    UFUNCTION()
    void OnImageSlot7Clicked();

    UFUNCTION()
    void OnImageSlot8Clicked();

    /** 버튼 클릭 이벤트 */
    UFUNCTION()
    void OnImageSlotClicked(int32 ButtonIndex);

    TArray<int32> HighlightedButtons;

};
