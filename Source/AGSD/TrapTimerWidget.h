// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TrapTimerWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class AGSD_API UTrapTimerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    /** 남은 시간을 갱신 (초 단위 정수) */
    UFUNCTION(BlueprintCallable, Category = "TrapTimer")
    void SetSeconds(int32 Seconds);

protected:
    virtual void NativeConstruct() override;

    /** 디자이너에서 배치: 중앙 상단의 TextBlock. 이름 반드시 동일하게 바인딩 */
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerText;

};
