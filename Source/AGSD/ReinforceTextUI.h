// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "WeaponDataTableBeta.h"
#include "ReinforceTextUI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UReinforceTextUI : public UUserWidget
{
    GENERATED_BODY()

public:
    // 버튼 위젯 변수
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    class UButton* YesButton;

    UFUNCTION(BlueprintCallable)
    void ReinforceWeapon();

    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;
	
};
