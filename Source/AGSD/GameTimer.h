// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameTimer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class AGSD_API UGameTimer : public UObject
{
	GENERATED_BODY()
	
    UGameTimer();

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float InitTime;
    //구버전용 시간 설정 제거 고려중


    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FTimerHandle GameTimer;
    //타이머 핸들

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int UITime;
    //정수형 시간의 흐름

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FString StringTime=" ";
    //실질적 UI출력용 문자열

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool TimeEnd = false;
    //시간 종료여부

    UFUNCTION(BlueprintCallable)
    void TimeSet(float Time);
    //레벨 시작시 시간 설정

    UFUNCTION(BlueprintCallable)
    void UpdateTime();
    //1초당 UITime업데이트 및 문자열 변환

    UFUNCTION(BlueprintCallable)
    void TimeOver();
    //시간 종료시 호출되는 함수

    UFUNCTION()
    FString MinutesSeconds(int Time);
    //시간을 분초로 변환

};
