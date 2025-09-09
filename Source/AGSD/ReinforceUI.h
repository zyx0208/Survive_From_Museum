// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponDataTableBeta.h"
#include "ReinforceUI.generated.h"

/**
 *
 */
UCLASS()
class AGSD_API UReinforceUI : public UUserWidget
{
    GENERATED_BODY()

public:
    // UUserWidget의 NativeConstruct 오버라이드
    virtual void NativeConstruct() override;

    /** 무기 아이콘을 UI에 표시 */
    void DisplayWeaponImage(UTexture2D* WeaponIcon1, UTexture2D* WeaponIcon2);

    /** UI를 닫는 함수 */
    UFUNCTION(BlueprintCallable, Category = "UI")
    void CloseUI();

    // 버튼 위젯 변수
    UPROPERTY(meta = (BindWidget))
    class UButton* AgreeButton;

    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot1;

    UPROPERTY(meta = (BindWidget))
    UImage* ImageSlot2;

    UPROPERTY(meta = (BindWidget))
    UImage* ArrowImage;

    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponDescription1;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponDescription2;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* Question;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponNameTextBlock1;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* WeaponNameTextBlock2;


    UFUNCTION()
    void OnAgreeButtonClicked();

    UFUNCTION()
    bool UpgradeCheck(FWeaponDataTableBetaStruct WeaponData);

    UFUNCTION()
    void Reinforce(FWeaponDataTableBetaStruct WeaponData);

    UFUNCTION()
    void ShowReinforce(FWeaponDataTableBetaStruct WeaponData);

    FWeaponDataTableBetaStruct* TargetWeapon;
    FWeaponDataTableBetaStruct* OriginalWeapon;

    int OriginalWeaponID;
    int TargetWeaponID;

    bool Recheck=false;

    bool WeaponArraySlot=false;
};
