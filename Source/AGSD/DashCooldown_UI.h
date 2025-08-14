// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DashCooldown_UI.generated.h"

/**
 * 
 */
UCLASS()
class AGSD_API UDashCooldown_UI : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /** UI 업데이트 함수 */
    //UFUNCTION(BlueprintCallable, Category = "Cooldown")
    //void UpdateDashCooldown(float CooldownPercentage);
    void UpdateAttackCooldown(float CooldownPercentage);
    void UpdateWeaponIcon();
    void UpdateSwapWeapon();

    void UpdatePrimeZCooldown(float CooldownPercentage);
    void UpdatePrimeXCooldown(float CooldownPercentage);

    /** 무기 데이터 테이블 */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UDataTable* WeaponDataTableBeta;

protected:
    /** 위젯 바인딩 */
    /*UPROPERTY(meta = (BindWidget))
    class UImage* DashIcon;*/

    UPROPERTY(meta = (BindWidget))
    class UImage* MainWeaponIcon;
    UPROPERTY(meta = (BindWidget))
    class UImage* SubWeaponIcon;

    /*UPROPERTY(meta = (BindWidget))
    class UProgressBar* DashCooldownBar;*/
    UPROPERTY(meta=(BindWidget))
    class UProgressBar* AttackCooldownBar;

    UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
    class UProgressBar* PrimeZCooldownBar;

    UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
    class UProgressBar* PrimeXCooldownBar;

private:
    bool mainicon = true;
};
